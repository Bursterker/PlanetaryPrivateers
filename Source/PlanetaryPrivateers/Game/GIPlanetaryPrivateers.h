// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GIPlanetaryPrivateers.generated.h"

/**
 * 
 */
UCLASS()
class PLANETARYPRIVATEERS_API UGIPlanetaryPrivateers : public UGameInstance
{
public:
	GENERATED_UCLASS_BODY()

	class AGSPlanetaryPrivateers* GetGameSession();
	/**
	 * Call upon the GameSession to host a game and start it
	 * @Param							bIsLan							True if the game should be hosted over LAN
	 * @Param							MaxNumPlayers			Maximum allowed players in the session
	 */
	UFUNCTION(BlueprintCallable, Category = "Networking|Lobby")
		void StartGame(bool bIsLan, int MaxNumPlayers);

	/**
	 * Call upon the GameSession to search for hosting games
	 * @Param						bIsLan								True to search for LAN games
	 */
	UFUNCTION(BlueprintCallable, Category = "Networking|Lobby")
		void FindGames(bool bIsLan);

	/**Call upon the GameSession to join a found session*/
	UFUNCTION(BlueprintCallable, Category = "Networking|Lobby")
		void JoinGame();

	/**Call upon the GameSession to terminate the current session connection*/
	UFUNCTION(BlueprintCallable, Category = "Networking|Lobby")
		void LeaveGame();

public:
	int32 m_NumPlayersThatTravelToNextGameMode = 0;
};
