// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "TimerManager.h"

#include "PuzzlePlatformsGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);
	if (NewPlayer == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("NewPlayer cannot be null."));
		return; //Exit since we don't want to overcount
	}

	PlayerCount++;
	UE_LOG(LogTemp, Error, TEXT("Player %s joined. %d players now in lobby"), *NewPlayer->GetName(), PlayerCount);

	if (PlayerCount >= 2) {
		UE_LOG(LogTemp, Error, TEXT("Lobby is full, starting game..."), *NewPlayer->GetName(), PlayerCount);
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartGame, StartGameDelay);
	}
}

void ALobbyGameMode::StartGame() {
	auto GameInstance = Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());
	if (!ensure(GameInstance != nullptr)) return;
	GameInstance->StartSession();

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	// Enable seamless travel - ie. use transition 'loading' map
	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/PuzzlePlatforms/Maps/Game?listen");
}

void ALobbyGameMode::Logout(AController* Exiting) {
	Super::Logout(Exiting);
	if (Exiting == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Exiting player controller is null."));
		//Don't exit, still want to decrement playercount
	}

	PlayerCount--;
	UE_LOG(LogTemp, Error, TEXT("Player %s left. %d players now in lobby"), *Exiting->GetName(), PlayerCount);
}