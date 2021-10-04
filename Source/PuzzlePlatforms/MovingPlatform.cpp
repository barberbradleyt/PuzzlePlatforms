// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform() {
	// Enable tick
	PrimaryActorTick.bCanEverTick = true;

	// Ensure component is Movable
	SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) { // Only replicate from server
		GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
		GlobalStartLocation = GetActorLocation();

		// Replicate location from server to client
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void AMovingPlatform::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (ActiveTriggers) {
		if (HasAuthority()) { // Only move on server
			FVector Location = GetActorLocation();

			if (FVector::Distance(GlobalStartLocation, Location) >= FVector::Distance(GlobalStartLocation, GlobalTargetLocation)) {
				FVector tmp = GlobalStartLocation;
				GlobalStartLocation = GlobalTargetLocation;
				GlobalTargetLocation = tmp;
			}
			// Calculate and set new location of not at target location
			FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
			Location += ActiveTriggers * Speed * DeltaTime * Direction;
			SetActorLocation(Location);
		}
	}

	//bool IsServer = HasAuthority();
}

void AMovingPlatform::AddActiveTrigger() {
	ActiveTriggers++;
}

void AMovingPlatform::RemoveActiveTrigger() {
	if (ActiveTriggers > 0) ActiveTriggers--;
	else ActiveTriggers = 0;
}