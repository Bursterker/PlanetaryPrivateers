// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PSPlanetaryPrivateers.generated.h"

/**
 * 
 */
UCLASS()
class PLANETARYPRIVATEERS_API APSPlanetaryPrivateers : public APlayerState
{
	GENERATED_UCLASS_BODY()

public:
	/**Returns the teamID variable*/
	UFUNCTION(BlueprintCallable, Category = "Teams")
		int GetTeamId() {return m_TeamId; }

protected:
	/**
	 * Copies the value of variables to a new instance of the given PlayerState
	 * @Param						PlayerState						The new PlayerState
	 */
	virtual void CopyProperties(APlayerState* pPlayerState) override;

public:
	int m_TeamId = 0;
	int m_TeleporterId = 0;
};
