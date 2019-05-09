// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RepairHitTarget.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRepairSmack);

UCLASS(BlueprintType, Blueprintable)
class PLANETARYPRIVATEERS_API ARepairHitTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARepairHitTarget();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable,BlueprintReadWrite)
	FOnRepairSmack OnRepairSmack;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// questionable name
	UFUNCTION(BlueprintCallable)
	void RepairSmack(FVector impactVector, FVector hitLocation);

	// Use this function
	UFUNCTION(BlueprintCallable)
	void SetTargetActive(bool bActivate);

protected:

	UPROPERTY(EditInstanceOnly)
	bool m_bStartActive{ false };

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Active)
	bool m_bActive{ false };

	UFUNCTION(BlueprintNativeEvent)
	void OnRep_Active();
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void HandleRepairSmack(bool bSuccessful, FVector impactVector, FVector hitLocation);

	UFUNCTION(BlueprintCallable)
	void CallOnRepairSmackFromBP();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UBoxComponent* m_pHitBox{nullptr};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* m_pHitBoxMesh{nullptr};


	// Reset location
	//FTransform m_InitialRelativeTransform


	// ================
	// --- Criteria ---
	UPROPERTY(EditAnywhere)
	float m_RequiredImpactForce{ 200.f };

public:
	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
};
