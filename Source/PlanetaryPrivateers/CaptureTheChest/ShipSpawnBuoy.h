// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShipSpawnBuoy.generated.h"

UCLASS()
class PLANETARYPRIVATEERS_API AShipSpawnBuoy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShipSpawnBuoy();


	TSubclassOf<AActor> GetShipClass();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//UPROPERTY(EditInstanceOnly, Category = "Spawn Settings", meta = (ToolTip = "Which team should this ship belong to"))
	//int32 m_TeamId{ 0 };
	//UPROPERTY(EditInstanceOnly, Category = "Spawn Settings", meta = (ToolTip = "Which ship should be spawned"))
	//TSubclassOf<AActor> m_ShipClass;

};
