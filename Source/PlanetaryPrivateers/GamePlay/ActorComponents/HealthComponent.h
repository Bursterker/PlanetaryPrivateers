// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthZero);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecoverFromZeroHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChange, int32, newHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageTaken, int32, dmg);

class UPrimitiveComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLANETARYPRIVATEERS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	UFUNCTION(BlueprintCallable)
	void DealDamage(int32 dmg);

	/**
	 * Function that will only deal damage if the provided hitbox is linked with this health component
	 * Returns whether or not the provided hitbox is linked
	 * 
	 * Combination of GetIsLinkedToHitBox() and DealDamage()
	 */
	UFUNCTION(BlueprintCallable)
	bool DealDamageToHitBox(UPrimitiveComponent* pHitBox, int32 dmg);

	UPROPERTY(BlueprintAssignable)
	FOnHealthZero OnHealthZero;
	UPROPERTY(BlueprintAssignable)
	FOnRecoverFromZeroHealth OnRecoverFromZeroHealth;
	UPROPERTY(BlueprintAssignable)
	FOnHealthChange OnHealthChange;
	UPROPERTY(BlueprintAssignable)
	FOnDamageTaken OnDamageTaken;

	/**
	 * Function that sets the health to the maximum value
	 * Use this when you want to replenish health
	 */
	UFUNCTION(BlueprintCallable)
	void HealToFullHealth();

	UFUNCTION(BlueprintCallable)
	int32 GetMaxHealth() { return m_MaxHealth; }
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentHealth() { return m_CurrentHealth; }

	UFUNCTION(BlueprintCallable)
	void LinkHitBox(UPrimitiveComponent* pHitBox);
	UFUNCTION(BlueprintCallable)
	bool GetIsLinkedToHitBox(UPrimitiveComponent* pHitBox);

private:
	
	UPROPERTY(Replicated, EditAnywhere)
	int32 m_MaxHealth{ 100 };
	UPROPERTY(Replicated)
	int32 m_CurrentHealth{ 0 };

	void UpdateHealth(int32 newHealth);


	TArray<UPrimitiveComponent*> m_tpHitBoxes{};

protected:
	// ===============
	// --- Team ID ---

	// NOTE: Currently not replicated > only valid for server
	UPROPERTY(BlueprintReadWrite)
	int32 m_TeamId{ -1 };
public:
	UFUNCTION(BlueprintCallable)
	int32 GetTeamId() { return m_TeamId; }
	UFUNCTION(BlueprintCallable)
	void SetTeamId(int32 teamId) { m_TeamId = teamId; }

public:
	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

};
