// Fill out your copyright notice in the Description page of Project Settings.

#include "GMLobby.h"
#include "Lobby/TeamTeleporter.h"
#include "Player/VRPawn.h"
#include "Player/PSPlanetaryPrivateers.h"
#include "Engine/Engine.h"

// ===================================================
void AGMLobby::AddTeleporter(ATeamTeleporter* Teleporter)
{
	m_tpTeleporters.Add(Teleporter);
}

// ===================================================
void AGMLobby::Tick(float DeltaSeconds)
{
	CheckAllTeleportersForPlayers();

	if (CheckIfTravelCanHappend())
	{
		TravelToNewMap();
	}
}

// ===================================================
void AGMLobby::CheckAllTeleportersForPlayers()
{
	m_NumTeleportersWithPlayers = 0;

	for(ATeamTeleporter* teleporter : m_tpTeleporters)
	{
		if (teleporter->m_HasPlayer)
		{
			m_NumTeleportersWithPlayers++;
		}
	}
}

// ===================================================
bool AGMLobby::CheckIfTravelCanHappend()
{
	//Check if all players are in seperate teleporters
	if (m_NumTeleportersWithPlayers == GetNumPlayers() && GetNumPlayers() > 0)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("GMLobby::CheckIfTravelCanHappend --> Enough tele's and players are valid")));
		//Create a 2D array to store the players in teams
		TArray<TArray<AVRPawn*>> teamsArray = {};
		for (int i = 0; i < m_AmmountOfTeams; ++i)
		{
			teamsArray.Add({});
		}

		//Loop over each teleporter and add the player on it to its team
		for (ATeamTeleporter* teleporter : m_tpTeleporters)
		{
			if (teleporter->m_HasPlayer == false)
			{
				continue;
			}

			//Get the teleporter teamId
			int32 teamId = teleporter->m_TeamId;

			//Get the player and it it to the team
			teamsArray[teamId].Add(teleporter->GetOverlappingPlayer());

			//Set the teleporter Id for after the travel
			APSPlanetaryPrivateers* playerState = dynamic_cast<APSPlanetaryPrivateers*>(teleporter->GetOverlappingPlayer()->GetPlayerState());
			playerState->m_TeleporterId = teleporter->m_Id;
		}

		//Check if each team has the same ammount of players
		int remainderOfPlayers = GetNumPlayers() % m_AmmountOfTeams;
		int minPlayersPerTeam = (GetNumPlayers() - remainderOfPlayers) / m_AmmountOfTeams;
		bool TeamsAreEven = true;

		for (TArray<AVRPawn*> teamArray : teamsArray)
		{
			if (teamArray.Num() < minPlayersPerTeam)
			{
				if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("GMLobby::CheckIfTravelCanHappend --> Teams are not even, return false if not allowing0players")));
				TeamsAreEven = false;
				if(!m_Allow0Players)
					return false;
			}
		}

		if (TeamsAreEven || (m_Allow0Players && (teamsArray[0].Num() >0 || teamsArray[1].Num() > 0)))
		{
			if(GEngine)GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("GMLobby.cpp::CheckIfTravelCanHappend --> All checkers have their required players/ Teams are even.")));

			//Assign each player their team in their playerstate
			for (int i = 0; i < teamsArray.Num(); ++i)
			{
				for (AVRPawn* player : teamsArray[i])
				{
					if (player)
					{
						APSPlanetaryPrivateers* playerState = dynamic_cast<APSPlanetaryPrivateers*>(player->GetPlayerState());
						playerState->m_TeamId = i;
					}
					else
					{
						if (!m_Allow0Players) return false;
					}
				}
			}
			return true;
		}
	}
	else { if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("GMLobby::CheckIfTravelCanHappend --> Not enough teleporters are tagged with a player compared to the num players ingame."))); }
	return false;
}

// ===================================================
void AGMLobby::TravelToNewMap()
{
	/**Transport players to second phase*/
	bUseSeamlessTravel = true;
	GetWorld()->ServerTravel(m_MapToTravelTo);
}