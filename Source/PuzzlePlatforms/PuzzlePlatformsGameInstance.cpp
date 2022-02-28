// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"

#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"

const static FName SESSION_NAME = TEXT("My Game Session - 112bvvg47fg");
const static FName CUSTOM_NAME_KEY = TEXT("CustomSessionName");

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer& ObjectInitializer) {
	ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (!ensure(MainMenuBPClass.Class != nullptr)) return;
	MainMenuClass = MainMenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> PauseMenuBPClass(TEXT("/Game/MenuSystem/WBP_PauseMenu"));
	if (!ensure(PauseMenuBPClass.Class != nullptr)) return;
	PauseMenuClass = PauseMenuBPClass.Class;
}

void UPuzzlePlatformsGameInstance::Init() {
	Super::Init();
	UE_LOG(LogTemp, Display, TEXT("Found main menu class '%s'."), *MainMenuClass->GetName());

	//Temporarily retrieve OnlineSubsystem
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Online subsystem returned nullptr."));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Found online subsystem '%s'."), *OnlineSubsystem->GetSubsystemName().ToString());

	//Store SessionInterface
	SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("No valid session interface found."));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Found session interface."));
	IsLAN = IOnlineSubsystem::Get()->GetSubsystemName().ToString() == "NULL";

	//Session event handlers required since session operations are asynchronous
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
	SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
}

void UPuzzlePlatformsGameInstance::LoadMenu() {
	if (!ensure(MainMenuClass != nullptr)) return;

	Menu = CreateWidget<UMainMenu>(this, MainMenuClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();
	Menu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::LoadPauseMenu() {
	if (!ensure(PauseMenuClass != nullptr)) return;

	UMenuWidget* PauseMenu = CreateWidget<UMenuWidget>(this, PauseMenuClass);
	if (!ensure(PauseMenu != nullptr)) return;

	PauseMenu->Setup();
	PauseMenu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::Host(FString LobbyName) {
	if (SessionInterface.IsValid()) {
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Existing session found. ReCreating..."));
			ReCreateSession();
		}
		else CreateSession(LobbyName);
	}
}

void UPuzzlePlatformsGameInstance::CreateSession(FString LobbyName) {
	UE_LOG(LogTemp, Display, TEXT("Creating session..."));

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = IsLAN;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true; // Use lobbies
	//SessionSettings.bUseLobbiesIfAvailable = true; // Use lobbies in UE >= 4.27
	SessionSettings.Set(CUSTOM_NAME_KEY, LobbyName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UPuzzlePlatformsGameInstance::ReCreateSession() {
	//OnDestroySessionComplete handler will call CreateSession asynchronously
	UE_LOG(LogTemp, Display, TEXT("Destroying session..."));
	SessionInterface->DestroySession(SESSION_NAME);
}

void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success) {
	if (!Success) {
		UE_LOG(LogTemp, Error, TEXT("Failed to create game session."));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Session creation complete."));

	if (Menu != nullptr) Menu->Teardown();

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Hosting..."));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	World->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
}

void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success) {
	if (!Success) {
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy game session."));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Session destruction complete."));

	//TODO: This should re-use the custom lobby name
	CreateSession(TEXT("Recreated Session"));
}

void UPuzzlePlatformsGameInstance::RefreshServerList() {
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (!SessionSearch.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("Session search not valid."));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Searching for sessions..."));
	//SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UPuzzlePlatformsGameInstance::OnFindSessionComplete(bool bWasSuccessful) {
	UE_LOG(LogTemp, Display, TEXT("Session search complete. Printing results..."));
	
	if (!bWasSuccessful || !SessionSearch.IsValid() || Menu == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Find session failed."));
		return;
	}

	auto Results = SessionSearch->SearchResults;
	if (Results.Num() <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("No sessions found."));
		
		// For list testing
		/*
		TArray<FString> TestNames;
		TestNames.Add("Test Result 1");
		TestNames.Add("Test Result 2");
		Menu->SetServerList(TestNames);
		*/
		//

		return;
	}

	//Update ServerList on Join Menu 
	TArray<FServerData> ServerData;
	for (const FOnlineSessionSearchResult& Result : Results) {
		if (!Result.IsValid()) {
			UE_LOG(LogTemp, Error, TEXT("Search result invalid."));
		}

		FServerData Server;
		if (!Result.Session.SessionSettings.Get(CUSTOM_NAME_KEY, Server.Name)) {
			Server.Name = Result.GetSessionIdStr();
		}
		Server.Hostname = Result.Session.OwningUserName;
		Server.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
		Server.CurrentPlayers = Server.MaxPlayers - Result.Session.NumOpenPublicConnections;

		//Debugging
		UE_LOG(LogTemp, Warning, TEXT("Session Name = %s"), *Server.Name);
		UE_LOG(LogTemp, Warning, TEXT("Owning Username = %s"), *Server.Hostname);
		UE_LOG(LogTemp, Warning, TEXT("CurrentPlayers = %d"), Server.CurrentPlayers);
		UE_LOG(LogTemp, Warning, TEXT("MaxPlayers = %d"), Server.MaxPlayers);
		//

		ServerData.Add(Server);
	}

	Menu->SetServerList(ServerData);
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index) {
	UE_LOG(LogTemp, Warning, TEXT("Joining session [SessionIndex=%d]..."), Index);

	if (!SessionInterface.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("SessionInterface is not valid."));
		return;
	}
	if (!SessionSearch.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("SessionSearch is not valid."));
		return;
	}

	if (Menu != nullptr) {
		Menu->Teardown();
	}

	UE_LOG(LogTemp, Warning, TEXT("Calling SessionInterface->JoinSession..."));
	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
	UE_LOG(LogTemp, Warning, TEXT("Awaiting response..."));
}

void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) {
	UE_LOG(LogTemp, Warning, TEXT("Response received. Processing..."));

	// Ensure Result is successful
	if (Result == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Join result was NULL."));
		return;
	}

	
	if (IsLAN) { //Result does not come back Success for LAN
		UE_LOG(LogTemp, Error, TEXT("Joining LAN session."));
	}
	else if (Result != EOnJoinSessionCompleteResult::Type::Success) {
		switch (Result)
		{
		case EOnJoinSessionCompleteResult::SessionIsFull:
			UE_LOG(LogTemp, Error, TEXT("Session is full. Unable to join session."));
			return;
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			UE_LOG(LogTemp, Error, TEXT("Session does not exist."));
			return;
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
			UE_LOG(LogTemp, Error, TEXT("Could not retrieve session address. Unable to join session."));
			return;
		case EOnJoinSessionCompleteResult::AlreadyInSession:
			UE_LOG(LogTemp, Error, TEXT("Already in selected session."));
			return;
		case EOnJoinSessionCompleteResult::UnknownError:
			UE_LOG(LogTemp, Error, TEXT("Unknown error. Unable to join session."));
			return;
		default:
			break;
		}
	}

	// Get connection info 
	if (!SessionInterface.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("SessionInterface is not valid."));
		return;
	}
	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address)) {
		UE_LOG(LogTemp, Warning, TEXT("Could not retrieve connection string."));
	}

	// Connect
	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) {
		UE_LOG(LogTemp, Error, TEXT("Engine pointer is NULL."));
		return;
	}
	Engine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Joining %s..."), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) {
		UE_LOG(LogTemp, Error, TEXT("Player controller is NULL."));
		return;
	}
	PlayerController->ClientTravel(*Address, TRAVEL_Absolute);
	Engine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Successfully joined"), *Address));
}

void UPuzzlePlatformsGameInstance::LoadMainMenu() {
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;
	PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", TRAVEL_Absolute);
}
