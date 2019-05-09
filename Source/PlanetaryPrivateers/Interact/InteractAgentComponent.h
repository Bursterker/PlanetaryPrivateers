// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractAgentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAgentInteractStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAgentInteractStop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInterrupted);

class UInteractableComponent;
class UPrimitiveComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLANETARYPRIVATEERS_API UInteractAgentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractAgentComponent();

	
	UFUNCTION(BlueprintCallable)
	bool InteractWith(UInteractableComponent* pInteractableCmp);
	UFUNCTION(BlueprintCallable)
	bool InteractWithNearest(UPrimitiveComponent* pInteractingPrimitive);	// InteractAgent Collider
	//UFUNCTION(BlueprintCallable)
	bool InteractWithNearest();	
	//UFUNCTION(BlueprintCallable)
	void StopInteracting(bool bNotifyInteractable);
	
	void InterruptInteraction();
	UPROPERTY(BlueprintAssignable, Category = "Overridable Event")
	FOnInterrupted OnInterruptedEvent;
	UPROPERTY(BlueprintAssignable, Category = "Overridable Event")
	FOnAgentInteractStart OnAgentInteractStartEvent;
	UPROPERTY(BlueprintAssignable, Category = "Overridable Event")
	FOnAgentInteractStop OnAgentInteractStopEvent;


	UFUNCTION(BlueprintCallable)
	void LinkWithPrimitive(UPrimitiveComponent* pAgentPrimitive);
	UFUNCTION(BlueprintCallable)
	UPrimitiveComponent* GetPrimitiveAgent() { return m_pAgentPrimitive; }
	UFUNCTION(BlueprintCallable)
	UInteractableComponent* GetInteractingWith() { return m_pInteractingWith; }

private:

	UPrimitiveComponent* m_pAgentPrimitive{ nullptr };
	UInteractableComponent* m_pInteractingWith{ nullptr };
};
