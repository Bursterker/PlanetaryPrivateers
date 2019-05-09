// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCannon.h"
#include "Particles/ParticleSystemComponent.h"

// ===================================================
// Sets default values
ABaseCannon::ABaseCannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setup the particle system
	m_FireParticleEmitter = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticleEmitter"));
	m_FireParticleEmitter->DeactivateSystem();
}

// ===================================================
// Called when the game starts or when spawned
void ABaseCannon::BeginPlay()
{
	Super::BeginPlay();
	
}

// ===================================================
// Called every frame
void ABaseCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ===================================================
void ABaseCannon::MULTICAST_PlayFireEffects_Implementation()
{
	//Play emitter particle
	
	//Play Animation
}

// ===================================================
bool ABaseCannon::MULTICAST_PlayFireEffects_Validate()
{
	return true;
}