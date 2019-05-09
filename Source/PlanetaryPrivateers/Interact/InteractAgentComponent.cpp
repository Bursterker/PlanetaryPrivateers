// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractAgentComponent.h"
#include "Interact/InteractableComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UInteractAgentComponent::UInteractAgentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// ===================================================
bool UInteractAgentComponent::InteractWith(UInteractableComponent * pInteractable)
{
	bool bSuccess{ false };

	if (pInteractable)
	{
		bSuccess = pInteractable->StartInteraction(this);
		if (bSuccess)
		{
			m_pInteractingWith = pInteractable;
		}
	}

	return bSuccess;
}

// ===================================================
bool UInteractAgentComponent::InteractWithNearest(UPrimitiveComponent * pInteractingPrimitive)
{
	bool bSuccess{ false };

	if (pInteractingPrimitive)
	{	
		// 1. Get all overlapping colliders
		TSet<UPrimitiveComponent*> tpOverlappingColliders;
		pInteractingPrimitive->GetOverlappingComponents(tpOverlappingColliders);
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Interact overlap: %i components found"), tpOverlappingColliders.Num()));

		// 2. Find closest overlapping interactable collider
		float shortestDist{ MAX_flt };
		UInteractableComponent* pClosestInteractable{ nullptr };
		UPrimitiveComponent* pClosestInteractedCollider{ nullptr };

		// 2.1 Iterate over all overlapping colliders
		for (UPrimitiveComponent* pOverlappedCollider : tpOverlappingColliders)
		{
			// Check if is Interactable
			AActor* pOverlappedActor{ pOverlappedCollider->GetOwner() };
			
			TArray<UInteractableComponent*> tInteractableComponents;
			pOverlappedActor->GetComponents<UInteractableComponent>(tInteractableComponents);

			// Iterate over all UInteractableComponents of class until one found corresponding to the overlapping Collider
			for (UInteractableComponent* pInteractableCmp : tInteractableComponents)
			{
				bool bCorrectInteractable = pInteractableCmp->GetIsSameCollider(pOverlappedCollider);
				if (bCorrectInteractable)
				{
					float distSq = FVector::DistSquared(pInteractingPrimitive->GetComponentLocation(), pOverlappedCollider->GetComponentLocation());
					if (distSq < shortestDist)
					{
						// New shortest distance
						shortestDist = distSq;
						pClosestInteractable = pInteractableCmp;
					}
					break;
				}
			}
			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("V Name: %s"), *pOverlappedActor->GetHumanReadableName()));
		}

		if (pClosestInteractable)
		{
			InteractWith(pClosestInteractable);
			OnAgentInteractStartEvent.Broadcast();
			bSuccess = true;
		}
	}

	return bSuccess;
}

bool UInteractAgentComponent::InteractWithNearest()
{
	return InteractWithNearest(m_pAgentPrimitive);
}

// ===================================================
void UInteractAgentComponent::StopInteracting(bool bNotifyInteractable)
{
	if (m_pInteractingWith)
	{
		if (bNotifyInteractable)
		{
			m_pInteractingWith->StopInteraction(false);
		}
		
		m_pInteractingWith->StopInteraction();
		OnAgentInteractStopEvent.Broadcast();
		m_pInteractingWith = nullptr;
	}
}

// ===================================================
void UInteractAgentComponent::InterruptInteraction()
{
	OnInterruptedEvent.Broadcast();
	m_pInteractingWith = nullptr;
}

void UInteractAgentComponent::LinkWithPrimitive(UPrimitiveComponent * pAgentPrimitive)
{
	m_pAgentPrimitive = pAgentPrimitive;
}
