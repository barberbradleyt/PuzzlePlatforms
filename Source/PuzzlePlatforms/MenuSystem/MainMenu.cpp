// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "UObject/ConstructorHelpers.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

#include "ServerResultWidget.h"

UMainMenu::UMainMenu(const FObjectInitializer & ObjectInitializer) {
	ConstructorHelpers::FClassFinder<UUserWidget> ServerResultBPClass(TEXT("/Game/MenuSystem/WBP_ServerResult"));
	if (!ensure(ServerResultBPClass.Class != nullptr)) return;

	UE_LOG(LogTemp, Warning, TEXT("ServerResultClass = %s"), *ServerResultClass);

	ServerResultClass = ServerResultBPClass.Class;
}

bool UMainMenu::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) return false;
	
	// TODO: setup
	if (!ensure(HostButton != nullptr)) return false;
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(JoinButton != nullptr)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(JoinWithIPButton != nullptr)) return false;
	JoinWithIPButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if (!ensure(BackToMainButton != nullptr)) return false;
	BackToMainButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitPressed);

	return true;
}

void UMainMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) {
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
	this->RemoveFromViewport();

	if (!ensure(InWorld != nullptr)) return;

	APlayerController* PlayerController = InWorld->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;
}

void UMainMenu::HostServer() {
	UE_LOG(LogTemp, Display, TEXT("Hosting Server..."));
	if (MainMenuInterface == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("MainMenuInterface is null"));
		return;
	}
	MainMenuInterface->Host();
}

void UMainMenu::SetServerList(TArray<FString> ServerNames) {
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	ServerResults->ClearChildren();
	uint32 current_index = 0;
	for (const FString& ServerName : ServerNames) {
		UServerResultWidget* ServerResult = CreateWidget<UServerResultWidget>(World, ServerResultClass);
		if (!ensure(ServerResult != nullptr)) return;

		ServerResult->SetServerName(ServerName);
		ServerResult->Setup(this, current_index++);

		UE_LOG(LogTemp, Warning, TEXT("Adding new ServerResult to ServerResults"));
		ServerResults->AddChild(ServerResult);
	}
}

void UMainMenu::SelectIndex(uint32 Index) {
	SelectedIndex = Index;
}

void UMainMenu::OpenMainMenu() {
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(MainMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OpenJoinMenu() {
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);
	if (MainMenuInterface != nullptr) {
		MainMenuInterface->RefreshServerList();
	}
}

void UMainMenu::JoinServer() {
	if (SelectedIndex.IsSet() && MainMenuInterface != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("SelectedIndex=%d"), SelectedIndex.GetValue());
		MainMenuInterface->Join(SelectedIndex.GetValue());
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("SelectedIndex not set"));
	}
	//FString IP = "1.0.0.27";
	//MainMenuInterface->Join(IP);
}

void UMainMenu::QuitPressed() {
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ConsoleCommand("quit");
}