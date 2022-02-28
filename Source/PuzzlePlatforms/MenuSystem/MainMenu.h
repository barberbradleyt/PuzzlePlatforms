// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"


USTRUCT()
struct FServerData {
	GENERATED_BODY()

	FString Name;
	FString Hostname;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
};

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

public:
	UMainMenu(const FObjectInitializer& ObjectInitializer);

	void SetServerList(TArray<FServerData> ServerData);

	void SelectIndex(uint32 Index);

protected:
	virtual bool Initialize() override;

	void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

private:
	TSubclassOf<UUserWidget> ServerResultClass;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* CustomLobbyName;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostLobbyButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackFromHostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinWithIPButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackFromJoinButton;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerResults;

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void QuitPressed();

	TOptional<uint32> SelectedIndex;

	void UpdateChildren();
};
