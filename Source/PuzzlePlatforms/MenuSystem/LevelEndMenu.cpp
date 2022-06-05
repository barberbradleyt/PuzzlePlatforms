// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelEndMenu.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

bool ULevelEndMenu::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) return false;

	// TODO: setup
	if (!ensure(RestartButton != nullptr)) return false;
	RestartButton->OnClicked.AddDynamic(this, &ULevelEndMenu::RestartLevel);

	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &ULevelEndMenu::LeaveServer);

	return true;
}

void ULevelEndMenu::SetWinningPlayer(AActor* WinningPlayer) {
	if (WinningPlayer != nullptr) {
		WinningPlayerName->SetText(FText::FromString(WinningPlayer->GetName()));
	}
}

void ULevelEndMenu::RestartLevel() {
	// Restart Level
}

void ULevelEndMenu::LeaveServer() {
	if (MainMenuInterface != nullptr) {
		Teardown();
		MainMenuInterface->LoadMainMenu();
	}
}