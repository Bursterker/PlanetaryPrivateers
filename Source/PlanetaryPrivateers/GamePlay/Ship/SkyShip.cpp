// Fill out your copyright notice in the Description page of Project Settings.

#include "SkyShip.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "UnrealNetwork.h"
#include "ShipSteeringWheel.h"
#include "ShipLever.h"

#include "Runtime/Engine/Classes/Components/ChildActorComponent.h"
#include "Components/TextRenderComponent.h"
#include "CaptureTheChest/GMCaptureTheChest.h"
#include <string>

ASkyShip::ASkyShip()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// 1. Ship Mesh
	m_pShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	m_pShipMesh->SetupAttachment(RootComponent);
	m_pShipMesh->SetCollisionProfileName("NoCollision");

	bReplicates = true;
}

void ASkyShip::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// Hard to work with in multiplayer (because of spawning actors)
	return;



	// Spawn Steering wheel and speed lever
	UWorld* pWorld{ GetWorld() };
	if (pWorld)
	{
		if (!m_SteeringWheelClass)
		{
			m_SteeringWheelClass = AShipSteeringWheel::StaticClass();
		}
		if (!m_SpeedLeverClass)
		{
			m_SpeedLeverClass = AShipLever::StaticClass();
		}

		// Spawn actors
		FTransform spawnTransform{};
		
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.bNoFail = true;

		// 1. Steering wheel
		spawnTransform.SetLocation(FVector(0.f, 0.f, 100.f));
		spawnTransform.SetRotation(FQuat(FVector::UpVector, 90.f));
		m_pSteeringWheel = pWorld->SpawnActor<AShipSteeringWheel>(m_SteeringWheelClass,spawnTransform, spawnParams);// Correct their location in blueprints
		
		// 2. Speed lever
		m_pSpeedLever = pWorld->SpawnActor<AShipLever>(m_SpeedLeverClass, spawnTransform, spawnParams);


		// Attach actors
		FAttachmentTransformRules attachRules{ FAttachmentTransformRules::SnapToTargetNotIncludingScale };
		m_pSteeringWheel->AttachToComponent(m_pShipMesh, attachRules);
		m_pSpeedLever->AttachToComponent(m_pShipMesh, attachRules);


		//if (m_pSteeringWheel)
		//{
		//	if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Emerald, TEXT("Spawned steering wheel < ASkyShip::PostInitializeComponents()"));
		//}
	}
	else
	{
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("World is nullptr while trying to spawn (child) actors < ASkyShip::PostInitializeComponents()"));
	}
}

// ===================================================
void ASkyShip::BeginPlay()
{
	Super::BeginPlay();

	// 1. Set Health - server only
	//if (HasAuthority())
	//{
	//	SERVER_SetHealth(m_MaxHealth);
	//}
}

// ===================================================
void ASkyShip::Tick(float dt)
{
	Super::Tick(dt);

	// Tick according to state
	//switch (m_ShipState)
	//{
	//case EShipState::BrokenState:
	//	StateTickBroken(dt);
	//	break;
	//case EShipState::Controllable:
	//	StateTickControllable(dt);
	//	break;
	//default:
	//	break;
	//}

	Movement(dt);

}

void ASkyShip::Movement_Implementation(float dt)
{
}

#pragma region Health
//// ===================================================
//void ASkyShip::SERVER_DealDamage_Implementation(float dmg)
//{
//	if (m_bImmuneToDamage)
//	{
//		return;
//	}
//
//	// Skip negative dmg values
//	if (dmg >= 0.f)
//	{
//		float newHealth = m_CurrentHealth - dmg;
//		SERVER_SetHealth(newHealth);
//	}
//}
//bool ASkyShip::SERVER_DealDamage_Validate(float dmg)
//{
//	return true;
//}
//
//// ===================================================
//void ASkyShip::SERVER_SetHealth_Implementation(float newHealth)
//{
//	// 1. Update Health value
//	m_CurrentHealth = FMath::Clamp<float>(newHealth, 0, m_MaxHealth);
//	
//	// 2. Check for specific events to trigger according to health
//
//	// - Enter BrokenState if health == 0
//	if (m_CurrentHealth <= 0.f)
//	{
//		SERVER_ChangeShipState(EShipState::BrokenState);
//
//		// Extra check 
//		if (m_MaxHealth <= 0.f)
//		{
//			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ASkyShip::m_MaxHealth is smaller or equal to zero. Ship will constantly break after repair duration. Change this. < ASkyShip::SetHealth()"));
//		}
//	}
//}
//
//bool ASkyShip::SERVER_SetHealth_Validate(float newHealth)
//{
//	return true;
//}
#pragma endregion Health


#pragma region Ship State
//// ===================================================
//void ASkyShip::SERVER_ChangeShipState_Implementation(EShipState newState)
//{
//	StateExit();
//	m_ShipState = newState;
//	StateEntry();
//}
//bool ASkyShip::SERVER_ChangeShipState_Validate(EShipState newState)
//{
//	return true;
//}
//
//// ===================================================
//void ASkyShip::StateEntry_Implementation()
//{
//	// Server only
//	// Don't call this from remote (so can't be a RPC)
//	if (!HasAuthority())
//	{
//		return;
//	}
//
//	switch (m_ShipState)
//	{
//	case EShipState::BrokenState:	
//	{
//		// 1. Set timer to become controllable again
//		FTimerManager& timerManager{ GetWorld()->GetTimerManager() };
//		// Make delegate
//		FTimerDelegate brokenTimerCallBack;
//		brokenTimerCallBack.BindLambda([this] {SERVER_ChangeShipState(EShipState::Controllable); });
//		// Set timer
//		timerManager.SetTimer(m_BrokenTimerHndl, brokenTimerCallBack, m_BrokenDuration, false);
//
//		// 2. Set immune to damage
//		//m_bImmuneToDamage = true;
//
//		// 3. On Ship broken blueprint and C++ functionality
//		OnShipBroken();
//	}
//		break;
//	case EShipState::Controllable:
//		// 1. Reset Health
//		//SERVER_SetHealth(m_MaxHealth);
//
//		// 2. Set NOT immune to damage
//		//m_bImmuneToDamage = false;
//
//		break;
//	default:
//		break;
//	}
//}
//
//// ===================================================
//void ASkyShip::StateExit_Implementation()
//{
//	// Server only
//// Don't call this from remote (so can't be a RPC)
//	if (!HasAuthority())
//	{
//		return;
//	}
//
//	switch (m_ShipState)
//	{
//	case EShipState::BrokenState:
//	{
//		
//		break;
//	}
//	case EShipState::Controllable:
//	{
//
//		break;
//	}
//	default:
//		break;
//	}
//}
//
//// ===================================================
//void ASkyShip::StateTickControllable_Implementation(float dt)
//{
//	if (HasAuthority())
//	{
//		Movement(dt);
//	}
//}
//
//// ===================================================
//void ASkyShip::StateTickBroken_Implementation(float dt)
//{
//}
//
//// ===================================================
//void ASkyShip::OnShipBroken_Implementation()
//{
//	// Note: Currently only called from server
//	// You can change this 
//	SERVER_RemoveCargo();
//}
#pragma endregion Ship State

// ===================================================
void ASkyShip::SERVER_RemoveCargo_Implementation()
{
	return; // don't use this function
	//Check if there is a chest in the cargo
	ATreasureChest* chest = GetCargo<ATreasureChest>();

	if(chest)
	{
		//Remove the chest from the ship
		chest->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		//Determine the direction the chest has to go in
		FVector direction = FVector(FMath::RandRange(0.f, 1.f), FMath::RandRange(0.5, 1.f), FMath::RandRange(0.f, 1.f));
		direction.Normalize();
		dynamic_cast<UStaticMeshComponent*>(chest->GetRootComponent())->AddImpulse(direction * m_ImpulseForce);
	}
}

// ===================================================
bool ASkyShip::SERVER_RemoveCargo_Validate()
{
	return true;
}

// ===================================================
void ASkyShip::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ASkyShip, m_ShipState);
	//DOREPLIFETIME(ASkyShip, m_CurrentHealth);
	//DOREPLIFETIME(ASkyShip, m_bImmuneToDamage);
}
