// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChestSpawner.generated.h"

UCLASS()
class PLANETARYPRIVATEERS_API AChestSpawner : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AChestSpawner();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**Spawns an actor of ATreasureChest on self location*/
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_SpawnTreasureChest();

private:
	/**Add self to the gamemode*/
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_AddSelfToGameMode();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ATreasureChest> m_TreasureChestClass;
};
