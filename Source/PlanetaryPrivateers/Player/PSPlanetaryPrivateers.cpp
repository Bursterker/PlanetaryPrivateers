// Fill out your copyright notice in the Description page of Project Settings.

#include "PSPlanetaryPrivateers.h"
#include "UnrealNetwork.h"

// ===================================================
APSPlanetaryPrivateers::APSPlanetaryPrivateers(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

// ===================================================
void APSPlanetaryPrivateers::CopyProperties(class APlayerState* pPlayerState)
{
	APlayerState::CopyProperties(pPlayerState);

	if (pPlayerState != nullptr)
	{
		APSPlanetaryPrivateers* playerState = Cast<APSPlanetaryPrivateers>(pPlayerState);
		if (playerState)
		{
			playerState->m_TeamId = m_TeamId;
			playerState->m_TeleporterId = m_TeleporterId;
		}
	}
}