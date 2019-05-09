// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableComponent.h"
#include "Interact/InteractAgentComponent.h"
#include "Runtime/Engine/Classes/Components/PrimitiveComponent.h"

// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	

	// ...
}

bool UInteractableComponent::StartInteraction(UInteractAgentComponent * pInteractAgent)
{
	bool bSuccess{ false };

	if(pInteractAgent)
	{
		if (m_pInteractedBy)
		{
			if (m_bInterruptable)
			{
				// -- Scenario A -- 
				// -- Interactable is currently Interacted by other InteractAgent + Interaction can be interrupted
				// > Eject old agent
				// > StartIntection with new agent

				StopInteraction(true);
				//m_pInteractedBy->InterruptInteraction();
				SetNewInteractionInfo(pInteractAgent);
				OnInteractStartEvent.Broadcast(this, m_pInteractedBy);

				// Can't be toggled

				bSuccess = true;
				return bSuccess;
			}

			else
			{
				// -- Scenario B -- 
				// -- Interactable is currently Interacted by other InteractAgent + Interaction CANNOT be interrupted
				// > Keep interacting with old Agent
				// > Reject this Interact attempt

				bSuccess = false;
				return bSuccess;
			}
		}

		else
		{
			// -- Scenario C -- 
			// -- Not being interacted with
			// > Allow interaction with new agent

			// Cache interact info
			SetNewInteractionInfo(pInteractAgent);
			// BroadCast start interact
			OnInteractStartEvent.Broadcast(this, m_pInteractedBy);

			if (m_bIsToggle)
			{
				StopInteraction();
			}

			bSuccess = true;
			return bSuccess;
		}
	}

	else
	{
		bSuccess = false;
		return bSuccess;
	}
}

void UInteractableComponent::StopInteraction(bool bNotifyAgent)
{
	if (m_pInteractedBy)
	{
		if (bNotifyAgent)
		{
			m_pInteractedBy->StopInteracting(false);	// Agent should not notify this interactable again
		}

		OnInteractStopEvent.Broadcast();
		ForgetInteractInfo();
	}
}

void UInteractableComponent::LinkWithCollider(UPrimitiveComponent * pCollider)
{
	m_tpLinedInteractableCol.AddUnique(pCollider);

	// Auto set collider to "Interactable" Profile (only overlap with InteractAgents)
	if (pCollider)
	{
		pCollider->SetCollisionProfileName("Interactable");
	}
}

void UInteractableComponent::SetNewInteractionInfo(UInteractAgentComponent * pInteractedBy)
{
	m_pInteractedBy = pInteractedBy;
}

void UInteractableComponent::ForgetInteractInfo()
{
	m_pInteractedBy = nullptr;
}
