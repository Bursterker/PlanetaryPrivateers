// Fill out your copyright notice in the Description page of Project Settings.

#include "TreasureChest.h"
#include "Components/StaticMeshComponent.h"
#include "GamePlay/Cannon/BaseProjectile.h"
#include "Player/PSPlanetaryPrivateers.h"
#include "GMCaptureTheChest.h"
#include "GamePlay/Ship/BaseShip.h"
#include "Components/BoxComponent.h"
#include "Player/VRPawn.h"
#include "Engine/Engine.h"
#include "Particles/ParticleSystemComponent.h"
#include "UnrealNetwork.h"

// ===================================================
// Sets default values
ATreasureChest::ATreasureChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	m_pHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	RootComponent = m_pHitBox;

	// Only check for collision on server
	m_pHitBox->SetCollisionProfileName(HasAuthority() ? "OverlapAllDynamic" : "NoCollision");

	//RegisterEvents
	//m_pHitBox->OnComponentBeginOverlap.AddDynamic(this, &ATreasureChest::OnOverlapBegin);

	//Particles
	m_pParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle system"));
	m_pParticleSystem->SetTemplate(m_pParticleInWorld);
	m_pParticleSystem->SetupAttachment(m_pHitBox);

}

// ===================================================
// Called when the game starts or when spawned
void ATreasureChest::BeginPlay()
{
	Super::BeginPlay();
	m_pParticleSystem->SetTemplate(m_pParticleInWorld);
}

// ===================================================
// Called every frame
void ATreasureChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ===================================================
void ATreasureChest::OnOverlapBegin(UPrimitiveComponent* pThisComponent, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult)//(AActor* OverlappedActor, AActor* OtherActor)
{
	
}

// ===================================================
void ATreasureChest::PlaceChestOnShip(ABaseProjectile* pHookProjectile)
{
	if (pHookProjectile)
	{
		//Check if the projectile comes from a harpoon
		AActor* pHarpoon = pHookProjectile->ObjectThatFiredMe;	// Harpoonlauncher does not intherit from ABaseCannon

		if (pHarpoon)
		{
			AVRPawn* playerThatFired = pHookProjectile->PlayerThatFiredMe;

			if (playerThatFired)
			{
				//Get the teamId of the player that fired the projectile
				m_OwningTeamId = dynamic_cast<APSPlanetaryPrivateers*>(playerThatFired->GetPlayerState())->GetTeamId();

				//Spawn the chest on the boat of the players team that hit the chest
				AGMCaptureTheChest* pCaptureTheChestGM{ dynamic_cast<AGMCaptureTheChest*>(GetWorld()->GetAuthGameMode()) };
				if (pCaptureTheChestGM)
				{
					ABaseShip* pShip = pCaptureTheChestGM->GetShipOfTeam(m_OwningTeamId);
					if (pShip)
					{
						pShip->PlaceActorOnShip(this);
						//AttachToComponent(pShip->GetCargo(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
						//this->AttachToActor(pShip, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
						//this->SetActorTransform(pShip->GetCargoSpace());
						SERVER_SwitchParticle(m_pParticleOnShip);
					}
				}
			}
			else
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(3, 10.f, FColor::Red, FString::Printf(TEXT("TreasureChest.cpp::PlaceChestOnShip --> PlayerThatFiredMe is nullptr")));
				}
			}
		}
	}
}

// ===================================================
void ATreasureChest::SERVER_SwitchParticle_Implementation(UParticleSystem* pParticleSystem)
{
	m_pParticleSystem->SetTemplate(pParticleSystem);
}

// ===================================================
bool ATreasureChest::SERVER_SwitchParticle_Validate(UParticleSystem* pParticleSystem)
{
	return true;
}

// ===================================================
void ATreasureChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATreasureChest, m_pParticleSystem);
}
