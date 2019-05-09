// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

class UPrimitiveComponent;
class UInteractAgentComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractStart, UInteractableComponent*, pInteractableCmp, UInteractAgentComponent*, pInteractAgent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractStop);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLANETARYPRIVATEERS_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractableComponent();
	UFUNCTION(BlueprintCallable)
	bool StartInteraction(UInteractAgentComponent* pInteractAgent);
	UFUNCTION(BlueprintCallable)
	void StopInteraction(bool bNotifyAgent = false);	// See this as: does the StopInteraction originate from this interactableComponent? Yes > tell Agent to stop aswell -- No > Don't tell agent

	UFUNCTION(BlueprintCallable)
	UInteractAgentComponent* GetInteractedBy() { return m_pInteractedBy; }

	UFUNCTION(BlueprintCallable)
	void LinkWithCollider(UPrimitiveComponent* pCollider);

	UPROPERTY(BlueprintAssignable, Category = "Overridable Events")
	FOnInteractStart OnInteractStartEvent;
	UPROPERTY(BlueprintAssignable, Category = "Overridable Events")
	FOnInteractStop OnInteractStopEvent;

	bool IsBeingInteractedWith() { return m_pInteractedBy != nullptr; }

	UFUNCTION(BlueprintCallable)
	bool GetIsSameCollider(const UPrimitiveComponent* pInteractableCollider) { return m_tpLinedInteractableCol.Contains(pInteractableCollider); }

	UFUNCTION(BlueprintCallable)
	TArray<UPrimitiveComponent*> GetInteractablePrimitves() { return m_tpLinedInteractableCol; }

protected:

	// Button = toggle, CannonHolding != toggle
	UPROPERTY(EditAnywhere)
	bool m_bIsToggle{ false }; 
	UPROPERTY(EditAnywhere)
	bool m_bInterruptable{ false };

private:
	
	void SetNewInteractionInfo(UInteractAgentComponent* pInteractedBy);
	void ForgetInteractInfo();

	UInteractAgentComponent* m_pInteractedBy{ nullptr };

	// The collider that this UInteractableComponent is linked with
	TArray<UPrimitiveComponent*> m_tpLinedInteractableCol;
	//UPrimitiveComponent* m_pLinkedInteractableCol{ nullptr };
};
