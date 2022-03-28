// Copyright Epic Games, Inc. All Rights Reserved.

#include "PuzzlePlatformsGameMode.h"
#include "PuzzlePlatformsCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/GameSession.h"

APuzzlePlatformsGameMode::APuzzlePlatformsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void APuzzlePlatformsGameMode::PlayerWon(AActor* Winner) {
	UE_LOG(LogTemp, Display, TEXT("Player %s won!!!"), *Winner->GetName());

	UGameInstance* GameInstance = GetGameInstance();
	if (!ensure(GameInstance != nullptr)) return;

	UEngine* Engine = GameInstance->GetEngine();
	if (!ensure(Engine != nullptr)) return;
	
	Engine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Player %s won!!!"), *Winner->GetName()));

	RestartGame();
}

void APuzzlePlatformsGameMode::RestartGame() {
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	if (GameSession != nullptr && GameSession->CanRestartGame() && World->IsServer())
	{
		GetWorld()->ServerTravel("?Restart");
	}
}