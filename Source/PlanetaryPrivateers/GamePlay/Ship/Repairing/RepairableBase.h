// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RepairableBase.generated.h"

class UBoxComponent;
class UHealthComponent;
class ARepairHitTarget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBreak); // Start being broken
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRepaired); // Stop being broken

UENUM(BlueprintType)
enum class ERepairState : uint8
{
	RepairedState, // Change this name
	BrokenState
};

UCLASS()
class PLANETARYPRIVATEERS_API ARepairableBase : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ARepairableBase();
	// Called every frame
	virtual void Tick(float dt) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	USceneComponent* m_pRoot {nullptr};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* m_pHitBox {nullptr};

	// ==============
	// --- Health ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthComponent* m_pHealthCmp {nullptr};
private:
	// State is directly dependant on the health of the HealthComponent
	UFUNCTION()
	void HandleHealthZero();
	UFUNCTION()
	void HandleHealthRecoverFromZero();


	// ==============
	// --- States ---
	ERepairState m_RepairState;

	UFUNCTION(NetMulticast,Reliable, WithValidation)
	void MC_ChangeState(ERepairState newState);
	void StateEntry();
	void StateExit();

protected:
	UFUNCTION(BlueprintNativeEvent)
	void StateRepairedTick(float dt);
	UFUNCTION(BlueprintNativeEvent)
	void StateBrokenTick(float dt);

public:
	UFUNCTION(BlueprintCallable)
	ERepairState GetRepairState() { return m_RepairState; }
	UPROPERTY(BlueprintAssignable)
	FOnRepaired OnRepaired;
	UPROPERTY(BlueprintAssignable)
	FOnBreak OnBreak;



	// =======================
	// --- Repair settings ---
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Repair Settings", meta = (ClampMin = 0))
	int32 m_MaxRepairSteps{ 3 };
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentRepairStep)
	int32 m_CurrentRepairStep{ 0 };

	UFUNCTION(BlueprintNativeEvent)
	void OnRep_CurrentRepairStep();


public:
	/**
	 * -- Should only be called on server --
	 * Function that adds a single repair step, working towards the max repair steps
	 * Will change state to HealthyState if CurrentSteps == MaxSteps
	 */
	UFUNCTION(BlueprintCallable)
	void Repair();

private:
	// Adds child repairtargets to the array
	void DetectRepairTargets();
	UPROPERTY(Replicated)
	TArray<ARepairHitTarget*> m_tpRepairTargets{};

	void ActivateRandomRepairTarget(ARepairHitTarget* pToExclude);
	ARepairHitTarget* m_pActiveRepairTarget{ nullptr };

	// Active target got succesfully hit and it deactivated itself
	// Unbind from event
	// Spawn new one if required
	UFUNCTION()
	void OnActiveRepairTargetSmacked();

public:

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
};
