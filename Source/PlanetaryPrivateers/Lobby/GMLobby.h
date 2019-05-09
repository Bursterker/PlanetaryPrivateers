// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/GMPlanetaryPrivateers.h"
#include "GMLobby.generated.h"

/**
 * 
 */
UCLASS()
class PLANETARYPRIVATEERS_API AGMLobby : public AGMPlanetaryPrivateers
{
	GENERATED_BODY()

public:
	void AddTeleporter(class ATeamTeleporter* Teleporter);

private:
	virtual void Tick(float DeltaSeconds) override;
	void CheckAllTeleportersForPlayers();
	bool CheckIfTravelCanHappend();
	void TravelToNewMap();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_Allow0Players = false;

private:
	TArray<class ATeamTeleporter*> m_tpTeleporters;
	int32 m_NumTeleportersWithPlayers = 0;
	int32 m_AmmountOfTeams = 2;
	FString m_MapToTravelTo = "/Game/Levels/CTFArena";
};
