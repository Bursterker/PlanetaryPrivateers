// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseGoal.h"
#include "Components/StaticMeshComponent.h"
#include "GamePlay/Ship/BaseShip.h"
#include "CaptureTheChest/TreasureChest.h"
#include "CaptureTheChest/GMCaptureTheChest.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "UnrealNetwork.h"

// ===================================================
// Sets default values
ABaseGoal::ABaseGoal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Goal"));
	m_pMesh->SetupAttachment(RootComponent);
	//Set up the collision presets
	//m_pMesh->SetCollisionProfileName("OverlapAll");
	
	m_pDeliveryHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("Hitbox"));
	m_pDeliveryHitBox->SetupAttachment(RootComponent);
	m_pDeliveryHitBox->SetCollisionProfileName("OverlapAll");
	m_pDeliveryHitBox->SetSphereRadius(500.f);

	if (HasAuthority())
	{
		m_pDeliveryHitBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseGoal::OnOverlapBegin);
	}

	//Particles
	m_pParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Scoring Particle"));
	m_pParticleSystem->SetupAttachment(m_pMesh);
	//m_pParticleSystem->DeactivateSystem();

	//RegisterEvents
	/*OnActorBeginOverlap.AddDynamic(this, &ABaseGoal::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ABaseGoal::OnOverlapEnd);*/
}

// ===================================================
// Called when the game starts or when spawned
void ABaseGoal::BeginPlay()
{
	Super::BeginPlay();
	m_pParticleSystem->DeactivateSystem();

}

// ===================================================
// Called every frame
void ABaseGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ===================================================
void ABaseGoal::OnOverlapBegin(class UPrimitiveComponent* pThisComponent, class AActor* pOtherActor, UPrimitiveComponent* pOtherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult)
{
	if (!HasAuthority()) return;

	//Check if OtherActor is a boat
	ABaseShip* pShip = dynamic_cast<ABaseShip*>(pOtherActor);

	if (pShip)
	{

		TArray<ATreasureChest*> tpTreasureChestsFromShip{ pShip->ReleaseAllTreasureCargo() };
		for (ATreasureChest* pTreasureChest : tpTreasureChestsFromShip)
		{

			//Add a point to the team
			int teamId = pTreasureChest->m_OwningTeamId;
			dynamic_cast<AGMCaptureTheChest*>(GetWorld()->GetAuthGameMode())->AddScoreOfTeam(teamId);

			//Destroy the chest
			pTreasureChest->Destroy();


			//Play the onscored effects
			SERVER_PlayOnScoreEffects();

			//Spawn a new chest
			// Should only be a single treasure chest
			dynamic_cast<AGMCaptureTheChest*>(GetWorld()->GetAuthGameMode())->SpawnChestRandomLoc();
		}
	}
}

// ===================================================
void ABaseGoal::SERVER_PlayOnScoreEffects_Implementation()
{
	m_pParticleSystem->ActivateSystem();
}

// ===================================================
bool ABaseGoal::SERVER_PlayOnScoreEffects_Validate()
{
	return true;
}

// ===================================================
void ABaseGoal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseGoal, m_pParticleSystem);
}
