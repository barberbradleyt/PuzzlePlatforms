// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleGoalTriggerVolume.h"

#include "PuzzlePlatformsGameMode.h"

#include "DrawDebugHelpers.h"

APuzzleGoalTriggerVolume::APuzzleGoalTriggerVolume() {
    OnActorBeginOverlap.AddDynamic(this, &APuzzleGoalTriggerVolume::OnOverlapBegin);
}

void APuzzleGoalTriggerVolume::BeginPlay()
{
	Super::BeginPlay();
	DrawDebugBox(GetWorld(), GetActorLocation(), GetActorScale() * 100, FColor::Cyan, true, -1, 0, 5);
}

void APuzzleGoalTriggerVolume::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor) {
	APuzzlePlatformsGameMode* GameMode = Cast<APuzzlePlatformsGameMode>(GetWorld()->GetAuthGameMode());
	if (!ensure(GameMode != nullptr)) return;

	GameMode->PlayerWon(OtherActor);
}