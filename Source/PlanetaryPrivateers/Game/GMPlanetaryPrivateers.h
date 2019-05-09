// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GMPlanetaryPrivateers.generated.h"

/**
 * 
 */
UCLASS()
class PLANETARYPRIVATEERS_API AGMPlanetaryPrivateers : public AGameMode
{
	GENERATED_BODY()

protected:

	/**Returns the game session class to use*/
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

	/**Code to be executed each game tick*/
	virtual void Tick(float DeltaSeconds) override;

	/**Called when a player connects to the gamemode*/
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/**Called before servertravel happends*/
	virtual void ProcessServerTravel(const FString& URL, bool bAbsolute) override;

public:
	/**Prints all the teamId's of the found players their PlayerState*/
	UFUNCTION(BlueprintCallable)
	void PrintAllPlayerTeamIds();

private:
	/**Copies information to the game instance*/
	void CopyInformationToGameInstance();

private:
	TArray<APlayerController*> m_tpConnectedPlayers;
};
