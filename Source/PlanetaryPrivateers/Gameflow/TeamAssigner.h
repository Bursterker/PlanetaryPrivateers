// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameflow/OverlappingCounter.h"
#include "TeamAssigner.generated.h"

/**
 * 
 */
UCLASS()
class PLANETARYPRIVATEERS_API ATeamAssigner : public AOverlappingCounter
{
	GENERATED_BODY()

public:

	/**
	 * Assing a team to all players overlapping with this trigger
	 * @Param							TeamId							Id to be assigned
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "Team")
		void SERVER_AssignTeam(int TeamId);
};
