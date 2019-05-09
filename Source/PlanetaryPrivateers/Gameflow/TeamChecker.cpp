// Fill out your copyright notice in the Description page of Project Settings.

#include "TeamChecker.h"
#include "TeamAssigner.h"
#include "Engine/Engine.h"
#include "Game/GSPlanetaryPrivateers.h"
#include "GameFramework/GameModeBase.h"
#include "Player/VRPawn.h"

// Sets default values
ATeamChecker::ATeamChecker()
{
	/**Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.*/
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(false);
}

// ===================================================
// Called when the game starts or when spawned
void ATeamChecker::BeginPlay()
{
	/**If the TeamChecker is on the server spawn the assigners*/
	if (HasAuthority())
	{
		SERVER_SpawnTeamAssigners();
	}

	Super::BeginPlay();
}

// ===================================================
// Called every frame
void ATeamChecker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		SERVER_CheckAndAssignTeam();
	}

	for (ATeamAssigner* teamAssigner : m_tpTeamAssigners)
	{
		teamAssigner->DrawDebugLines();
	}
}

// ===================================================
FOnlineSessionSettings* ATeamChecker::GetSessionSettings()
{
	IOnlineSessionPtr Sessions = IOnlineSubsystem::Get()->GetSessionInterface();
	return Sessions->GetSessionSettings(GameSessionName);
}

// ===================================================
void ATeamChecker::SetTriggersTransform(TArray<FTransform> Transforms)
{
	if (m_tpTeamAssigners.Num() != Transforms.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("ATeamChecker.cpp::SetTriggersTransform --> The passed ammount of transforms is not equal to the ammount of existing TeamAssigners")));
		return;
	}

	for(int32 i = 0; i < m_tpTeamAssigners.Num(); i++)
	{
		m_tpTeamAssigners[i]->SetActorTransform(Transforms[i]);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("ATeamChecker.cpp::SetTriggersTransform --> Transformed a TeamAssigner")));
	}
}

// ===================================================
void ATeamChecker::SERVER_CheckAndAssignTeam_Implementation()
{
	/**Check if all players are in the game*/
	if (m_AssignedTeams == false && m_CheckForTeams)
	{
		int playersInGame = GetWorld()->GetAuthGameMode()->GetNumPlayers();
		int remainderOfPlayers = playersInGame  % m_AmmountOfTeams;
		int minPlayersPerTeam = (playersInGame - remainderOfPlayers) / m_AmmountOfTeams;
		int playersFoundOverlapping = 0;

		/**Check each teamAssigner if it has the min required players*/
		for(ATeamAssigner* teamAssigner : m_tpTeamAssigners)
		{
			int32 playersOverlapping = teamAssigner->GetAllOverlappingOfClass<AVRPawn>().Num();
			if ( playersOverlapping>= minPlayersPerTeam)
			{
				playersFoundOverlapping += playersOverlapping;
			}
			else
			{
				return;
			}
		}

		/**Check if all the overlapping players are the total ammount of players in game*/
		bool allPlayersReady = false;

		if (playersFoundOverlapping == playersInGame)
		{
			allPlayersReady = true;
		}

		/**If the end is reached && allPlayersAreOverlapping --> all checkpoints have their required players, Assign TeamID's*/
		if (allPlayersReady)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("ATeamChecker.cpp::SERVER_CheckAndAssignTeam_Implementation --> All checkers have their required players.")));

			for (int i = 0; i < m_tpTeamAssigners.Num(); i++)
			{
				m_tpTeamAssigners[i]->SERVER_AssignTeam(i);
			}

			m_AssignedTeams = true;

			/**Transport players to second phase*/
			GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = true;
			GetWorld()->ServerTravel(m_MapToTravelTo);
		}
	} 
}

// ===================================================
bool ATeamChecker::SERVER_CheckAndAssignTeam_Validate()
{
	return true;
}

// ===================================================
void ATeamChecker::SERVER_SpawnTeamAssigners_Implementation()
{
	/**Create new instances of all needed teamAssigners*/
	for (int i = 0; i < m_AmmountOfTeams; ++i)
	{
		/**Spawn an actor of class and attach it to this*/
		ATeamAssigner* newTeamAssigner = GetWorld()->SpawnActor<ATeamAssigner>(FVector(0,0,0), FRotator(0,0,0));

		if(newTeamAssigner)
		{
			newTeamAssigner->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			m_tpTeamAssigners.Add(newTeamAssigner);
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("ATeamChecker.cpp::SERVER_SpawnTeamAssigners_Implementation --> Spawned a TeamAssigner.")));
		}
		else
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("ATeamChecker.cpp::SERVER_SpawnTeamAssigners_Implementation --> Failed to spawn a TeamAssigner.")));
	}
}

// ===================================================
bool ATeamChecker::SERVER_SpawnTeamAssigners_Validate()
{
	return true;
}

// ===================================================
void ATeamChecker::SetCheckForTeams(bool CheckForTeams)
{
	m_CheckForTeams = CheckForTeams;
}
