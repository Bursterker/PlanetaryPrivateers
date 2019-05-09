// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UnrealNetwork.h"

// ===================================================
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	m_CurrentHealth = m_MaxHealth;

	bReplicates = true;
}

// ===================================================
void UHealthComponent::LinkHitBox(UPrimitiveComponent * pHitBox)
{
	if (pHitBox)
	{
		m_tpHitBoxes.AddUnique(pHitBox);
		// Set hitbox to only overlap with DamageProjectiles
		pHitBox->SetCollisionProfileName("HealthHitBox");
	}
}

// ===================================================
bool UHealthComponent::GetIsLinkedToHitBox(UPrimitiveComponent * pHitBox)
{
	return m_tpHitBoxes.Contains<UPrimitiveComponent*>(pHitBox);
}

// ===================================================
void UHealthComponent::DealDamage(int32 dmg)
{
	// Discard negative damage values
	if (dmg >= 0)
	{
		int32 newHealth{ m_CurrentHealth - dmg };
		UpdateHealth(newHealth);
		OnDamageTaken.Broadcast(dmg);
	}
}

// ===================================================
bool UHealthComponent::DealDamageToHitBox(UPrimitiveComponent * pHitBox, int32 dmg)
{
	bool bIsLinkedToHitBox{ GetIsLinkedToHitBox(pHitBox) };
	if (bIsLinkedToHitBox)
	{
		DealDamage(dmg);
	}

	return bIsLinkedToHitBox;
}

// ===================================================
void UHealthComponent::HealToFullHealth()
{
	UpdateHealth(m_MaxHealth);
}

// ===================================================
void UHealthComponent::UpdateHealth(int32 newHealth)
{
	bool bHealthWasZero{ m_CurrentHealth == 0 };

	m_CurrentHealth = FMath::Clamp<int32>(newHealth, 0, m_MaxHealth);

	OnHealthChange.Broadcast(m_CurrentHealth);

	if (m_CurrentHealth == 0 && !bHealthWasZero)
	{
		// First frame zero health
		OnHealthZero.Broadcast();
	}
	else if (bHealthWasZero && m_CurrentHealth > 0)
	{
		// First frame alive/recovered from zero health
		OnRecoverFromZeroHealth.Broadcast();
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, m_MaxHealth);
	DOREPLIFETIME(UHealthComponent, m_CurrentHealth);
}
