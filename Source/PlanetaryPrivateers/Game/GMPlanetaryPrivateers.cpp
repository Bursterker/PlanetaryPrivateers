// Fill out your copyright notice in the Description page of Project Settings.

#include "GMPlanetaryPrivateers.h"
#include "GSPlanetaryPrivateers.h"
#include "Engine/Engine.h"
#include "GameFramework/GameState.h"
#include "Player/PSPlanetaryPrivateers.h"
#include "Game/GIPlanetaryPrivateers.h"

// ===================================================
TSubclassOf<AGameSession> AGMPlanetaryPrivateers::GetGameSessionClass() const
{
	return AGSPlanetaryPrivateers::StaticClass();
}

// ===================================================
void AGMPlanetaryPrivateers::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

// ===================================================
void AGMPlanetaryPrivateers::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("GMPlanetaryPrivateers::PostLogin --> A new player connected!")));

	m_tpConnectedPlayers.Add(NewPlayer);
}

// ===================================================
void AGMPlanetaryPrivateers::ProcessServerTravel(const FString& URL, bool bAbsolute)
{
	Super::ProcessServerTravel(URL, bAbsolute);

	CopyInformationToGameInstance();
}

// ===================================================
void AGMPlanetaryPrivateers::PrintAllPlayerTeamIds()
{
	/**Get all PlayersStates*/
	AGameState* gameState = dynamic_cast<AGMPlanetaryPrivateers*>(GetWorld()->GetAuthGameMode())->GetGameState<AGameState>();
	TArray<APlayerState*> playerStates = gameState->PlayerArray;

	/**For each playerState cast to PSPlanetaryPrivateers and print the team Id*/
	for(APlayerState* playerState :  playerStates)
	{
		int32 teamId = dynamic_cast<APSPlanetaryPrivateers*>(playerState)->GetTeamId();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, .5f, FColor::Blue, FString::Printf(TEXT("GMPlanetaryPrivateers.cpp::PrintAllPlayerIds --> Player: %s, TeamID: %d"), *playerState->GetPlayerName(), teamId));
			GEngine->AddOnScreenDebugMessage(-1, .5f, FColor::Blue, FString::Printf(TEXT("GMPlanetaryPrivateers.cpp::AssignTeam --> PlayerName: %s"), *playerState->GetPlayerName()));
		}
	}
}

// ===================================================
void AGMPlanetaryPrivateers::CopyInformationToGameInstance()
{
	UGIPlanetaryPrivateers* gameInstance =  dynamic_cast<UGIPlanetaryPrivateers*>(GetGameInstance());

	if (gameInstance)
	{
		if(m_tpConnectedPlayers.Num() == 0)
		{
			GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Red, FString::Printf(TEXT("GMPlanetaryPrivateers::CopyInformationToGameInstance --> There were no players connected throug the PostLogin, using GetNumPlayers")));
			gameInstance->m_NumPlayersThatTravelToNextGameMode = GetNumPlayers();
			GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Red, FString::Printf(TEXT("GMPlanetaryPrivateers::CopyInformationToGameInstance --> Number of players to be carried over to the next GameMode: %i"), GetNumPlayers()));
		}
		gameInstance->m_NumPlayersThatTravelToNextGameMode = m_tpConnectedPlayers.Num();
		GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Red, FString::Printf(TEXT("GMPlanetaryPrivateers::CopyInformationToGameInstance --> Number of players to be carried over to the next GameMode: %i"), m_tpConnectedPlayers.Num()));
	}
	else if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Red, FString::Printf(TEXT("GMPlanetaryPrivateers::CopyInformationToGameInstance --> The game instance was invalid!")));
	}
}
