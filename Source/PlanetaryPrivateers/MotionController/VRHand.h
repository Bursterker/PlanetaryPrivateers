// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionController/VRMotionController.h"
#include "VRHand.generated.h"

class USphereComponent;
class UInteractAgentComponent;
class USkeletalMesh;
/**
 * 
 */
UCLASS()
class PLANETARYPRIVATEERS_API AVRHand : public AVRMotionController
{
	GENERATED_BODY()

public:
	AVRHand();
	virtual void Tick(float DeltaTime) override;

	//---- Mesh ----
	void SetGender(bool bFemale);

	// ==================
	// ---- Interact ----

	void AttemptInteract();
	void StopInteract();

	// ==================
	// ---- Teleport ----

	bool CalculateTeleportLocation(FVector& outLocation);
	// Development only - Uses DrawDebugLine
	void EnableTeleportIndicator(bool bEnable) { m_bShowTeleportIndicator = bEnable; }
	UPROPERTY(EditAnywhere)
	float m_MaxTeleportDist{ 2000.f };


	UFUNCTION(BlueprintNativeEvent)
	void DbgInteract();

private:

	UPROPERTY(EditAnywhere)
	USkeletalMesh* m_pMaleHandMesh;
	UPROPERTY(EditAnywhere)
	USkeletalMesh* m_pFemaleHandMesh;

	// ==================
	// ---- Interact ----
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* m_pInteractTrigger;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInteractAgentComponent* m_pInteractAgent{ nullptr };

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AttemptInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopInteract();

public:
	UFUNCTION(BlueprintCallable)
	USphereComponent* GetInteractTrigger() { return m_pInteractTrigger; }
	
private:
	// ==================
	// ---- Teleport ----
	// Development only
	bool m_bShowTeleportIndicator{ false };


};
