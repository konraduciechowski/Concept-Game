// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStateManager.h"

// Sets default values
AWorldStateManager::AWorldStateManager(): bInterruptCurrentState(false) {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWorldStateManager::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void AWorldStateManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

bool AWorldStateManager::HasState(FString Key) {
	return States.Contains(Key);
}

void AWorldStateManager::AddState(FString Key, int32 Val) {
	States.Add(Key, Val);
}

void AWorldStateManager::RemoveState(FString Key) {
	if (States.Contains(Key)) {
		States.Remove(Key);
	}
}

void AWorldStateManager::ClearStates() {
	States.Empty();
}

void AWorldStateManager::SetState(FString Key, int32 Val) {
	if (States.Contains(Key)) {
		States[Key] = Val;
	}
	else {
		States.Add(Key, Val);
	}
}

void AWorldStateManager::ModifyState(FString Key, int32 Val) {
	if (States.Contains(Key)) {
		States[Key] += Val;
		if (States[Key] <= 0) {
			RemoveState(Key);
		}
	}
	else {
		States.Add(Key, Val);
	}
}

TMap<FString, int32> AWorldStateManager::GetStates() {
	return States;
}
