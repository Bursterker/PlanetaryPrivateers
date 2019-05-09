// Fill out your copyright notice in the Description page of Project Settings.

#include "ChestSpawner.h"
#include "TreasureChest.h"
#include "Engine/World.h"
#include "GMCaptureTheChest.h"
#include "Engine/Engine.h"

// ===================================================
// Sets default values for this component's properties
AChestSpawner::AChestSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ...
}


// ===================================================
// Called when the game starts
void AChestSpawner::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	SERVER_AddSelfToGameMode();
}

// ===================================================
// Called every frame
void AChestSpawner::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// ...
}

// ===================================================
void AChestSpawner::SERVER_SpawnTreasureChest_Implementation()
{
	ATreasureChest* chest = GetWorld()->SpawnActor<ATreasureChest>(m_TreasureChestClass, GetActorLocation(), FRotator::ZeroRotator);
	if(GEngine && chest)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("ChestSpawner.cpp::SERVER_SpanTreasureChest_Implementation --> Spawned a chest.")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("ChestSpawner.cpp::SERVER_SpanTreasureChest_Implementation --> Spawned chest was null")));

	}
}

// ===================================================
bool AChestSpawner::SERVER_SpawnTreasureChest_Validate()
{
	return true;
}

// ===================================================
void AChestSpawner::SERVER_AddSelfToGameMode_Implementation()
{
	dynamic_cast<AGMCaptureTheChest*>(GetWorld()->GetAuthGameMode())->AddSpawner(this);
}

// ===================================================
bool AChestSpawner::SERVER_AddSelfToGameMode_Validate()
{
	return true;
}

