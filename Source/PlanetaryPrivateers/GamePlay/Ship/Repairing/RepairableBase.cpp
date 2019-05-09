// Fill out your copyright notice in the Description page of Project Settings.

#include "RepairableBase.h"
#include "Components/BoxComponent.h"
#include "PlanetaryPrivateers/GamePlay/ActorComponents/HealthComponent.h"
#include "UnrealNetwork.h"
#include "Engine/Engine.h"
#include "PlanetaryPrivateers/Gameplay/Ship/Repairing/RepairHitTarget.h"

// ===================================================
ARepairableBase::ARepairableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 1. Root
	m_pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = m_pRoot;

	// 2.1 HitBox
	m_pHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	m_pHitBox->SetupAttachment(RootComponent);
	m_pHitBox->SetCollisionProfileName("HealthHitBox");

	// 2.2 Health
	m_pHealthCmp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthCmp"));
	AddOwnedComponent(m_pHealthCmp);
	if (HasAuthority())
	{
		m_pHealthCmp->OnHealthZero.AddDynamic(this, &ARepairableBase::HandleHealthZero);
		m_pHealthCmp->OnRecoverFromZeroHealth.AddDynamic(this, &ARepairableBase::HandleHealthRecoverFromZero);
	}
	m_pHealthCmp->LinkHitBox(m_pHitBox);

	bReplicates = true;
}

// ===================================================
void ARepairableBase::BeginPlay()
{
	Super::BeginPlay();

	// Create list of all repair targets attached to this actor 
	if (HasAuthority())
	{
		DetectRepairTargets();
	
		// Disable all detected target at start
		for (ARepairHitTarget* pTarget : m_tpRepairTargets)
		{
			// Local is used because if the server would call SetTargetActiveNet() here,
			// there is no guarantee that the clients will receive the multicast RPC called in SetTargetActiveNet
			pTarget->SetTargetActive(false);
		}
	}
}

// ===================================================
void ARepairableBase::Tick(float dt)
{
	Super::Tick(dt);

	switch (m_RepairState)
	{
	case ERepairState::RepairedState:
		// Do rotation

		// blueprint functionality
		StateRepairedTick(dt);

		break;
	case ERepairState::BrokenState:
		// Be broken
		// blueprint functionality
		StateBrokenTick(dt);


		break;
	default:
		break;
	}

}

// ===================================================
void ARepairableBase::DetectRepairTargets()
{
	TArray<AActor*> tpChildren;
	GetAttachedActors(tpChildren);
	for (AActor* pChild : tpChildren)
	{
		ARepairHitTarget* pTarget{ Cast<ARepairHitTarget>(pChild) };
		if (pTarget)
		{
			// Child is of class ARepairHitTarget
			m_tpRepairTargets.Add(pTarget);
		}
	}
}

// ===================================================
void ARepairableBase::ActivateRandomRepairTarget(ARepairHitTarget* pToExclude)
{
	if (m_CurrentRepairStep < m_MaxRepairSteps)
	{
		int32 nbRandomTargets{ m_tpRepairTargets.Num() };
		
		// Check if there are any targets
		if (nbRandomTargets > 0)
		{
			ARepairHitTarget* pSelectedTarget{ nullptr };
			while (pSelectedTarget == nullptr)
			{
				// Activate random
				int32 randomTargetIndex{ FMath::Rand() % nbRandomTargets };
				
				// Go throught with the target at the random target if:
				// 1. There is only a single target to chose from 
				// OR
				// 2. The random target is not the one that needs to be excluded
				if (m_tpRepairTargets[randomTargetIndex] != pToExclude || nbRandomTargets == 1)
				{
					pSelectedTarget = m_tpRepairTargets[randomTargetIndex];
				}
			}

			// Activate the nth element
			m_pActiveRepairTarget = pSelectedTarget;
			m_pActiveRepairTarget->SetTargetActive(true);

			// Bind on hit
			m_pActiveRepairTarget->OnRepairSmack.AddDynamic(this, &ARepairableBase::OnActiveRepairTargetSmacked);

		}
		else
		{
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("[Fix this] Server: there are no repair targets detected. This repairable can't be repaired. < ARepairableBase::ActivateRandomRepairTarget()"));
		}
	}

}

// ===================================================
void ARepairableBase::OnActiveRepairTargetSmacked()
{
	// Cache last active target
	ARepairHitTarget* pLastActiveTarget{ m_pActiveRepairTarget };

	// Unbind event
	m_pActiveRepairTarget->OnRepairSmack.Clear();
	m_pActiveRepairTarget = nullptr;

	Repair();

	// Check if the it is still broken
	if (m_RepairState == ERepairState::BrokenState)
	{
		// Still broken
		// Activate new repair target
		ActivateRandomRepairTarget(pLastActiveTarget);
	}
}

// ===================================================
void ARepairableBase::Repair()
{
	// Should be called on server only
	if (!HasAuthority())
		return;

	if (m_RepairState == ERepairState::BrokenState)
	{
		m_CurrentRepairStep = FMath::Clamp<int32>(m_CurrentRepairStep + 1, 0, m_MaxRepairSteps);
		//CurrentRepairStepsChanged();
		if (m_CurrentRepairStep == m_MaxRepairSteps)
		{
			// Change to Healthy state -> makes object controllable/working again
			// Should be called after the final repair animation is done?
			m_pHealthCmp->HealToFullHealth();
			
			//ChangeState(ERepairState::RepairedState);
		}
	}
}

// ===================================================
void ARepairableBase::MC_ChangeState_Implementation(ERepairState newState)
{
	StateExit();
	m_RepairState = newState;
	StateEntry();
}

bool ARepairableBase::MC_ChangeState_Validate(ERepairState newState)
{
	return true;
}

// ===================================================
void ARepairableBase::StateEntry()
{
	switch (m_RepairState)
	{
	case ERepairState::RepairedState:

		OnRepaired.Broadcast();

		

		break;
	case ERepairState::BrokenState:
		OnBreak.Broadcast();

		if (HasAuthority())
		{
			// Reset amount of steps needed to repair
			m_CurrentRepairStep = 0;
			// Start repair minigame loop
			ActivateRandomRepairTarget(nullptr);
		}

		break;
	default:
		break;
	}
}

// ===================================================
void ARepairableBase::StateExit()
{
	switch (m_RepairState)
	{
	case ERepairState::RepairedState:
		break;
	case ERepairState::BrokenState:
		break;
	default:
		break;
	}
}

// ===================================================
void ARepairableBase::HandleHealthZero()
{
	if (HasAuthority())
	{
		MC_ChangeState(ERepairState::BrokenState);
	}
}

// ===================================================
void ARepairableBase::HandleHealthRecoverFromZero()
{
	if (HasAuthority())
	{
		MC_ChangeState(ERepairState::RepairedState);
	}
}

// ===================================================
//void ARepairableBase::OnRep_CurrentRepairStep()
//{
//	//if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("OnRep_CurrentRepairSteps() gets called!"));
//	//return;
//	//// Blueprint call
//	//// This function doesn't get called?
//	//// Because The actor is owned by the server, so it won't call OnRep.. for clients?
//	CurrentRepairStepsChanged();
//}

// ===================================================
void ARepairableBase::OnRep_CurrentRepairStep_Implementation()
{

}

// ===================================================
void ARepairableBase::StateRepairedTick_Implementation(float dt)
{
}

// ===================================================
void ARepairableBase::StateBrokenTick_Implementation(float dt)
{
}

// ===================================================
void ARepairableBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARepairableBase, m_MaxRepairSteps);
	DOREPLIFETIME(ARepairableBase, m_CurrentRepairStep);
	DOREPLIFETIME(ARepairableBase, m_tpRepairTargets);
}