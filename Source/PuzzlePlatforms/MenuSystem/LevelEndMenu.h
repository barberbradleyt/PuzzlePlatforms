// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "LevelEndMenu.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API ULevelEndMenu : public UMenuWidget
{
	GENERATED_BODY()

public:
	void SetWinningPlayer(AActor* WinningPlayer);

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WinningPlayerName;

	UPROPERTY(meta = (BindWidget))
		class UButton* RestartButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;

	UFUNCTION()
		void RestartLevel();

	UFUNCTION()
		void LeaveServer();
};
