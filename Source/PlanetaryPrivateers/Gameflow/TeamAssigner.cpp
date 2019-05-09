// Fill out your copyright notice in the Description page of Project Settings.

#include "TeamAssigner.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "Player/PSPlanetaryPrivateers.h"
#include "GameFramework/GameModeBase.h"
#include "Player/VRPawn.h"

// ===================================================
void ATeamAssigner::SERVER_AssignTeam_Implementation(int TeamId)
{
	//Get all actors of the player class
	TArray<AVRPawn*> characters = GetAllOverlappingOfClass<AVRPawn>();

#pragma region Implementation
	if (characters.Num() > 0)
	{
		//For each found player
		for (AVRPawn* player : characters)
		{
			//Get the playerState
			APSPlanetaryPrivateers* playerState = dynamic_cast<APSPlanetaryPrivateers*> (player->GetPlayerState());

			if (playerState)
			{
				//Assign the TeamId
				playerState->m_TeamId = TeamId;
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("TeamAssigner.cpp::AssignTeam --> PlayerStateID: %d, Given ID: %d"), playerState->GetTeamId(), TeamId));
			}
			else if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("TeamAssigner.cpp::AssignTeam --> The playerstate is nullptr")));
			}
		}
	}
#pragma endregion
}

// ===================================================
bool ATeamAssigner::SERVER_AssignTeam_Validate(int TeamId)
{
	return true;
}

