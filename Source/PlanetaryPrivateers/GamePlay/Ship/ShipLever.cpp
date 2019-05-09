// Fill out your copyright notice in the Description page of Project Settings.

#include "ShipLever.h"
#include "UnrealNetwork.h"

// Sets default values
AShipLever::AShipLever()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShipLever::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShipLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ===================================================
void AShipLever::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShipLever, m_AxisValue);

}