// Fill out your copyright notice in the Description page of Project Settings.

#include "TeamTeleporter.h"
#include "Engine/World.h"
#include "GMLobby.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Player/VRPawn.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/Engine.h"

// ===================================================
// Sets default values
ATeamTeleporter::ATeamTeleporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	m_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	m_Mesh->SetupAttachment(GetRootComponent());
	m_Mesh->SetGenerateOverlapEvents(false);

	m_TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	m_TriggerBox->SetupAttachment(m_Mesh);
	m_TriggerBox->SetGenerateOverlapEvents(true);
	m_TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATeamTeleporter::OnOverlapBegin);
	m_TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ATeamTeleporter::OnOverlapEnd);

	m_pParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle system"));
	m_pParticleSystem->SetupAttachment(m_Mesh);
}

// ===================================================
// Called when the game starts or when spawned
void ATeamTeleporter::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
		AddSelfToGameMode();
}

// ===================================================
// Called every frame
void ATeamTeleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AVRPawn* ATeamTeleporter::GetOverlappingPlayer()
{
	for (AActor* actor : m_tpOverlappingActors)
	{
		AVRPawn* pawn = dynamic_cast<AVRPawn*>(actor);
		if (pawn)
		{
			return pawn;
		}
	}
	return nullptr;
}

// ===================================================
void ATeamTeleporter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	m_tpOverlappingActors.Add(OtherActor);
	if (GEngine) GEngine->AddOnScreenDebugMessage(1, 100.f, FColor::Blue, FString::Printf(TEXT("TeamTeleporter.cpp::OnOverlapBegin --> Overlapping now with: %s"), *OtherActor->GetName()));
	if (CheckIfAPlayerOverlaps()) m_HasPlayer = true;
}

// ===================================================
void ATeamTeleporter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	m_tpOverlappingActors.Remove(OtherActor);

	if (CheckIfAPlayerOverlaps() == false) m_HasPlayer = false;
}

// ===================================================
bool ATeamTeleporter::CheckIfAPlayerOverlaps()
{
	for(AActor* actor : m_tpOverlappingActors)
	{
		if (dynamic_cast<AVRPawn*>(actor))
		{
			return true;
		}
	}
	return false;
}

// ===================================================
void ATeamTeleporter::AddSelfToGameMode()
{
	AGMLobby* gameMode = dynamic_cast<AGMLobby*>(GetWorld()->GetAuthGameMode());

	if (gameMode)
	{
		gameMode->AddTeleporter(this);
	}
}

