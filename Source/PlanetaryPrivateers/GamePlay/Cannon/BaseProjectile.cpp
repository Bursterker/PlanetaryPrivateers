// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseProjectile.h"
#include "Components/PrimitiveComponent.h"
#include "UnrealNetwork.h"


// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bReplicateMovement = true;
}


// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseProjectile::AddImpulse(float force, FVector direction)
{
	// Only needs to be done on server
	UPrimitiveComponent* pBody{ Cast<UPrimitiveComponent>(GetComponentByClass(UPrimitiveComponent::StaticClass())) };
	if (pBody)
	{
		pBody->SetSimulatePhysics(true);
		pBody->AddImpulse(direction*force, NAME_None, true);
	}
}

void ABaseProjectile::MC_AddImpulse_Implementation(float force, FVector direction)
{
	AddImpulse(force, direction);
}
bool ABaseProjectile::MC_AddImpulse_Validate(float force, FVector direction)
{
	return true;
}

void ABaseProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseProjectile, ObjectThatFiredMe);
	DOREPLIFETIME(ABaseProjectile, PlayerThatFiredMe);
}

