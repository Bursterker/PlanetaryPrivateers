// Fill out your copyright notice in the Description page of Project Settings.

#include "AmmoContainerBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PlanetaryPrivateers/Interact/InteractableComponent.h"
#include "UnrealNetwork.h"


// Sets default values
AAmmoContainerBase::AAmmoContainerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;

	m_pContainerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ContainerMesh"));
	RootComponent = m_pContainerMesh;
	m_pContainerMesh->SetCollisionProfileName("BlockAllDynamic");
	
	m_pGrabTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("GrabTrigger"));
	m_pGrabTrigger->SetupAttachment(m_pContainerMesh);
	m_pGrabTrigger->SetCollisionProfileName(TEXT("Interactable"));

	m_pGrabInteractable = CreateDefaultSubobject<UInteractableComponent>(TEXT("GrabInteractable"));
	AddOwnedComponent(m_pGrabInteractable);

	
}

// Called when the game starts or when spawned
void AAmmoContainerBase::BeginPlay()
{
	Super::BeginPlay();
	m_CurrentAmmoLeft = m_MaxAmmoCapacity;
}

void AAmmoContainerBase::ConsumeAmmo()
{
	--m_CurrentAmmoLeft;

	OnAmmoLeftChanged.Broadcast(m_CurrentAmmoLeft);
	if (m_CurrentAmmoLeft == 0)
	{
		// Eject casing
		OnEmpty.Broadcast();
	}
}

// Called every frame
void AAmmoContainerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Tick may be disabled
}

void AAmmoContainerBase::LoadIntoCannon(AActor* pCannon)
{
	MC_LoadIntoCannon(pCannon);
}

void AAmmoContainerBase::LoadIntoCannonLocal_Implementation(AActor * pCannon)
{	
	// Should be called on server
	if (pCannon)
	{
		m_bLoaded = true;
		m_pLoadedInCannon = pCannon;
		m_pContainerMesh->SetSimulatePhysics(false);
		// Note this doesn't happen on client, will replicate movement work for this? If no > make own replicate transform functionality
	}
}	

void AAmmoContainerBase::MC_LoadIntoCannon_Implementation(AActor* pCannon)
{
	LoadIntoCannonLocal(pCannon);
}

bool AAmmoContainerBase::MC_LoadIntoCannon_Validate(AActor* pCannon)
{
	return true;
}

void AAmmoContainerBase::UnloadFromCannon(bool bEject, FVector ejectImpulse)
{
	MC_UnloadFromCannon(bEject, ejectImpulse);
}

void AAmmoContainerBase::EnablePhysics(bool bEnable)
{
	m_pContainerMesh->SetSimulatePhysics(bEnable);
	m_pContainerMesh->SetCollisionProfileName(bEnable ? FName("BlockAll") : FName("NoCollision"));
}

void AAmmoContainerBase::MC_UnloadFromCannon_Implementation(bool bEject, FVector ejectImpulse)
{
	UnloadFromCannonLocal(bEject);
}

bool AAmmoContainerBase::MC_UnloadFromCannon_Validate(bool bEject, FVector ejectImpulse)
{
	return true;
}

void AAmmoContainerBase::UnloadFromCannonLocal(bool bEject, FVector ejectImpulse)
{
	// Should be called on server
	m_bLoaded = false;
	m_pLoadedInCannon = nullptr;
	EnablePhysics(true);
	if (bEject)
	{
		FVector direction{ ejectImpulse };
		direction.Normalize();
		float force = ejectImpulse.Size();

		AddImpulse(force, direction);
	}
}

void AAmmoContainerBase::AddImpulse(float force, FVector direction)
{
	if (m_pContainerMesh->IsSimulatingPhysics())
	{
		m_pContainerMesh->AddImpulse(direction * force);
	}
}

void AAmmoContainerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAmmoContainerBase, m_CurrentAmmoLeft);
}
