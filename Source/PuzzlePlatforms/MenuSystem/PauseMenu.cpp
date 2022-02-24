// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu.h"

#include "Components/Button.h"

bool UPauseMenu::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) return false;

	// TODO: setup
	if (!ensure(ResumeButton != nullptr)) return false;
	ResumeButton->OnClicked.AddDynamic(this, &UPauseMenu::ClosePauseMenu);

	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UPauseMenu::LeaveServer);

	return true;
}

void UPauseMenu::ClosePauseMenu() {
	Teardown();
}

void UPauseMenu::LeaveServer() {
	if (MainMenuInterface != nullptr) {
		Teardown();
		MainMenuInterface->LoadMainMenu();
	}
}