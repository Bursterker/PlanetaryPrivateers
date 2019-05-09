// Fill out your copyright notice in the Description page of Project Settings.

#include "RepairHitTarget.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "UnrealNetwork.h"


// Sets default values
ARepairHitTarget::ARepairHitTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	RootComponent = m_pHitBox;
	m_pHitBox->SetCollisionProfileName("RepairHitBox");
	//m_pHitBox->OnComponentBeginOverlap.AddDynamic(this, &ARepairHitTarget::OnOverlapBegin);


	m_pHitBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	m_pHitBoxMesh->SetupAttachment(m_pHitBox);
	m_pHitBoxMesh->SetCollisionProfileName("NoCollision");

	bReplicates = true;
}

// Called when the game starts or when spawned
void ARepairHitTarget::BeginPlay()
{
	Super::BeginPlay();

	// Start inactive
	if (!HasAuthority() && !m_bStartActive)
	{
		// OnRep would not get called if m_bActive changed in server beginplay 
		m_bActive = false;
		OnRep_Active();
	}
}

// Called every frame
void ARepairHitTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARepairHitTarget::RepairSmack(FVector impactVector, FVector hitLocation)
{
	// Meant to be called on server

	// Prepare two direction vectors
	FVector targetDownDir{ GetActorUpVector() };
	FVector impactDir{ impactVector };
	impactDir.Normalize();

	// Calculate impact force
	float force{ impactVector.Size() };
	float dot{ FVector::DotProduct(targetDownDir, impactDir) };
	float impactForce{ dot * force };

	bool bSuccessful{ impactForce > m_RequiredImpactForce };
	
	HandleRepairSmack(bSuccessful, impactVector, hitLocation);
}

void ARepairHitTarget::SetTargetActive(bool bActivate)
{
	// Should be called on server
	m_bActive = bActivate;
	if (HasAuthority())
	{
		OnRep_Active();
	}
}


void ARepairHitTarget::OnRep_Active_Implementation()
{

	RootComponent->SetVisibility(m_bActive, true);
	SetActorEnableCollision(m_bActive);
}

void ARepairHitTarget::CallOnRepairSmackFromBP()
{
	OnRepairSmack.Broadcast();
}

void ARepairHitTarget::HandleRepairSmack_Implementation(bool bSuccessful, FVector impactVector, FVector hitLocation)
{
	if (bSuccessful)
	{
		SetTargetActive(false);
		OnRepairSmack.Broadcast();
	}
}

void ARepairHitTarget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARepairHitTarget, m_bActive);
}
