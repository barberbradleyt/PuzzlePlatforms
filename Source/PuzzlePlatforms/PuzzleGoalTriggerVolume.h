// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "PuzzleGoalTriggerVolume.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API APuzzleGoalTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	APuzzleGoalTriggerVolume();

	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);
};
