// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/PSPlanetaryPrivateers.h"
#include "PSCaptureTheChest.generated.h"



/**
 * 
 */
UCLASS()
class PLANETARYPRIVATEERS_API APSCaptureTheChest : public APSPlanetaryPrivateers
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetShip(AActor* pShip) { m_pShip = pShip; }
	UFUNCTION(BlueprintCallable)
	AActor* GetShip() { return m_pShip; }

protected:
	// Reference to the ship the player is attached to/belongs to
	AActor* m_pShip;

};
