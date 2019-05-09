// Fill out your copyright notice in the Description page of Project Settings.

#include "ShipSteeringWheel.h"
#include "UnrealNetwork.h"

// Sets default values
AShipSteeringWheel::AShipSteeringWheel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShipSteeringWheel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShipSteeringWheel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ===================================================
void AShipSteeringWheel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShipSteeringWheel, m_WheelAxis);
}
