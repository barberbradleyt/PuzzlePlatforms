// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MenuSystem/MainMenuInterface.h"
#include "PuzzlePlatformsGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UPuzzlePlatformsGameInstance : public UGameInstance, public IMainMenuInterface
{
	GENERATED_BODY()

public:
	UPuzzlePlatformsGameInstance(const FObjectInitializer & ObjectInitializer);
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void LoadMenu();

	UFUNCTION(BlueprintCallable)
	void LoadPauseMenu();

	UFUNCTION(BlueprintCallable)
	void LoadLevelEndMenu(AActor* WinningPlayer);

	UFUNCTION(Exec)
	void Host(FString LobbyName) override;

	UFUNCTION(Exec)
	void Join(uint32 Index) override;

	void StartSession();

	void RestartLevel();

	virtual void LoadMainMenu() override;

	void RefreshServerList() override;

private:
	bool IsLAN = false;

	TSubclassOf<UUserWidget> MainMenuClass;
	TSubclassOf<UUserWidget> PauseMenuClass;
	TSubclassOf<UUserWidget> LevelEndMenuClass;

	class UMainMenu* Menu;
	
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void CreateSession(FString LobbyName);
	void ReCreateSession();

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
};
