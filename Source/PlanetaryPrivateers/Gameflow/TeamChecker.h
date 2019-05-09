// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeamChecker.generated.h"

class ATeamAssigner;

UCLASS()
class PLANETARYPRIVATEERS_API ATeamChecker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeamChecker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**Returns the current session settings*/
	class FOnlineSessionSettings* GetSessionSettings();

	/**Checks if all players have chosen a team, then assigns the player that team*/
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_CheckAndAssignTeam();

	/**Instantiates the trigger for team assignment*/
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_SpawnTeamAssigners();

	/**
	 *Moves the location of the triggers
	 * @Param							Transforms							Wanted locations for the triggers
	 */
	UFUNCTION(BlueprintCallable)
		void SetTriggersTransform(TArray<FTransform> Transforms);

	/**
	 * Tells the checker to start searching for players in their triggers
	 * @Param							CheckForTeams					Wether to check for players in the triggers or not
	 */
	UFUNCTION(BlueprintCallable)
	void SetCheckForTeams(bool CheckForTeams);

public:
	TArray<ATeamAssigner*> m_tpTeamAssigners;
	int32 m_AmmountOfTeams = 2;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapTravel")
	FString m_MapToTravelTo = "/Game/Levels/CTFArena";

private:
	bool m_AssignedTeams = false;
	bool m_CheckForTeams = false;
};
