// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShipWheel.generated.h"

class UPhysicsConstraintComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UInteractableComponent;
class UInteractAgentComponent;
class USceneComponent;

UCLASS()
class PLANETARYPRIVATEERS_API AShipWheel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShipWheel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_pSMBase;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* m_pWheelAxisBody;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_pSMWheel;

	// Components may be changable in blueprints, will see
	UPROPERTY(VisibleAnywhere)
	UPhysicsConstraintComponent* m_pWheelAxisConstraint;
	UPROPERTY(VisibleAnywhere)
	UPhysicsConstraintComponent* m_pGrabConstraint;

private:
	/**
	 * Function that correctly calls UPhysicsConstraintComponent::SetConstrainedComponents()
	 * Should be called after Constructor (like BeginPlay()) to avoid "Both bodies are static" bug. (Eventhough they aren't)
	 */
	void SetupConstraintComponents();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	UPROPERTY()
	UInteractableComponent* m_pGrabInteractable;

private:
	// ==============
	// -- Grabbing --

	UFUNCTION()
	void GrabInteractStart(UInteractableComponent* pInteractableCmp, UInteractAgentComponent* pAgentCmp);
	UFUNCTION()
	void GrabInteractStop();
	
	/**
	 * Multi cast function that:
	 * 1. Sets up a physical constraint. Wheel will rotate following the primitive cmp
	 * 2. Flags whether or not it is grabbed
	 */
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MC_SetupGrabConstraint(UPrimitiveComponent* pGrabbedPrim);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MC_ReleaseGrabConstraint();

	/**
	 * Remote: Rotate wheel (visual only)
	 * Server: Calculate wheel axis 
	 */
	bool m_bGrabbed{ false };

	

public:
	// ================
	// -- Wheel axis --
	
	UFUNCTION(BlueprintCallable)
	float GetWheelAxis() { return m_WheelAxis; }

private:
	/**
	 * Meant only to be used on the server (server responsible for ship movement)
	 * Function that calculates the current wheel axis [-1,1]
	 * Depends on m_pWheelAxisConstraint::AngularSwing2Limit AND m_pWheelAxisConstraint::CurrentSwing2
	 */
	void CalculateWheelAxis();
	UPROPERTY(Replicated)
	float m_WheelAxis{ 0.f };

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheel Settings")
	float m_WheelAngularLimit{ 179.f };

};