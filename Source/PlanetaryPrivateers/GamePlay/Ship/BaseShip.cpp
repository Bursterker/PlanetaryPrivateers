// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseShip.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "PlanetaryPrivateers/CaptureTheChest/TreasureChest.h"


// ===================================================
// Sets default values
ABaseShip::ABaseShip()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pShipBody = CreateDefaultSubobject<UBoxComponent>(TEXT("ShipBody"));	// Primitive component must be root of ship
	RootComponent = m_pShipBody;
	m_pCargoSpace = CreateDefaultSubobject<USceneComponent>(TEXT("CargoSpace"));
	m_pCargoSpace->SetupAttachment(RootComponent);
	
	bReplicates = true;

	m_pSpawnLoc0 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLoc0"));
	m_pSpawnLoc0->SetupAttachment(m_pShipBody);
	m_tpSpawnLocations.Add(m_pSpawnLoc0);

	m_pSpawnLoc1 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLoc1"));
	m_pSpawnLoc1->SetupAttachment(m_pShipBody);
	m_tpSpawnLocations.Add(m_pSpawnLoc1);

	m_pSpawnLoc2 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLoc2"));
	m_pSpawnLoc2->SetupAttachment(m_pShipBody);
	m_tpSpawnLocations.Add(m_pSpawnLoc2);

	m_pSpawnLoc3 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLoc3"));
	m_pSpawnLoc3->SetupAttachment(m_pShipBody);
	m_tpSpawnLocations.Add(m_pSpawnLoc3);
}

// ===================================================
// Called when the game starts or when spawned
void ABaseShip::BeginPlay()
{
	Super::BeginPlay();
	
}

// ===================================================
// Called every frame
void ABaseShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseShip::PlaceActorOnShip_Implementation(AActor * pToAttach)
{
	FVector startLoc{ pToAttach->GetActorLocation() };
	pToAttach->AttachToComponent(m_pCargoSpace, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	FVector endLoc{ pToAttach->GetActorLocation() };

	if (pToAttach->GetAttachParentActor())
	{
		int x = 0;

	}
	else
	{
		pToAttach->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform);
		if (pToAttach->GetAttachParentActor())
		{
			int y = 0;
		}
	}

	DrawDebugLine(GetWorld(), startLoc, endLoc,FColor::Cyan, false, 1000.f);
}

// ===================================================
FTransform ABaseShip::GetCargoSpace()
{
	return m_pCargoSpace->GetComponentTransform();
}

TArray<ATreasureChest*> ABaseShip::ReleaseAllTreasureCargo()
{
	TArray<ATreasureChest*> tpTreasureChestsInCargo{};

	for (int32 i = 0; i < m_tpActorsInCargo.Num(); i++)
	{

		auto pTreasureChest{ Cast<ATreasureChest>(m_tpActorsInCargo[i]) };
		if (pTreasureChest)
		{
			tpTreasureChestsInCargo.Add(pTreasureChest);
		}
	
	}

	for (ATreasureChest* pCargoActor: tpTreasureChestsInCargo)
	{
		pCargoActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		m_tpActorsInCargo.Remove(Cast<AActor>(pCargoActor));
	}

	return tpTreasureChestsInCargo;
}

void ABaseShip::AddActorToCargo(AActor * pActor)
{
	if (pActor)
	{
		m_tpActorsInCargo.AddUnique(pActor); 
	}
}

