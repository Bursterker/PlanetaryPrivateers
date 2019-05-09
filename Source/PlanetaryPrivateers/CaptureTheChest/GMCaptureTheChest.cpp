

// Fill out your copyright notice in the Description page of Project Settings.

#include "GMCaptureTheChest.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine/Engine.h"
#include "PSCaptureTheChest.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include <functional>
#include "../Player/VRPlayerController.h"
#include "CaptureTheChest/ChestSpawner.h"
#include "PlanetaryPrivateers/GamePlay/Ship/BaseShip.h"
#include "Game/GIPlanetaryPrivateers.h"
#include "Player/VRPawn.h"

// Note:
// Instead of hardcoding spawn transforms (which will be the same for every map that uses that blueprint)
// Use objects on the map with Tags "Team0SpawnBeacon/Buoy" - let gamemode search for the correct actor placed in the scene
// Use that transform
// If 2 maps use the same GM blueprint - they will share the spawnTransform, which we don't want!

// Note:
// Attaching to ship with AVRPlayerController

// ===================================================
void AGMCaptureTheChest::BeginPlay()
{
	Super::BeginPlay();

	// 1. Spawn the ship corresponding to each team
	SpawnShips();

	// 2. Spawn the treasure chest
	//SpawnChestRandomLoc();

	//Set the gameTime
	m_RemainGameTimeInSec = m_TotalGameTimeInSec;
	m_bGameIsOver = false;
}

// ===================================================
void AGMCaptureTheChest::GenericPlayerInitialization(AController* pController)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("GMCaptureTheChest::GenericPlayerInitialization --> Entered!")));
	// -- Player connect and has loaded new level
	Super::GenericPlayerInitialization(pController);

	// 1. Save the connected controller
	APlayerController* pPlayerController{ dynamic_cast<APlayerController*>(pController) };
	if (pPlayerController)
	{
		KeepReferenceToPlayer(pPlayerController);
	}

	//1.5. Posses a VR Pawn
	if (m_tpVRPawnsOnServer.Num() > 0)
	{
		pController->UnPossess();
		pController->Possess(m_tpVRPawnsOnServer[m_NextUnassignedPawn]);
		m_NextUnassignedPawn++;
	}

	// 2. Try to attach Pawn to ship
	AVRPlayerController* pVRController{ dynamic_cast<AVRPlayerController*>(pController) };
	if (pVRController)
	{
		TryAttachPawnToShip(pVRController);
	}
	else
	{
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("[Warning] Server: Connected player controller not of type AVRPlayerController. Did not attach pawn to ship. < AGMCaptureTheChest::GenericPlayerInitialization() "));
	}

}

void AGMCaptureTheChest::KeepReferenceToPlayer(APlayerController * pController)
{
	if (!m_tpConnectedPlayerControllers.Contains<APlayerController*>(pController))
	{
		m_tpConnectedPlayerControllers.Add(pController);
	}
	else
	{
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("[Warning] Server: Player controller already kept refernce for < AGMCaptureTheChest::KeepReferenceToPlayer()"));
	}
}

void AGMCaptureTheChest::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("GMCaptureTheChest::PostLogIn --> Entered!")));
}

void AGMCaptureTheChest::TryAttachPawnToShip(AVRPlayerController* pVRController)
{	
	// Attach the pawn to the corresponding ship
	if (m_bAutoAttachToShip)
	{
		// Check if pawn is not null 
		APawn* pPawn{ pVRController->GetPawn() };
		if (pPawn)
		{
			// Pawn is valid > attach it to the ship
			AttachPawnToShip(pVRController);
		}
		else
		{
			// Pawn not yet valid > call function when the PC possess a pawn
			pVRController->OnInitialPossesPawn.AddDynamic(this, &AGMCaptureTheChest::AttachPawnToShip);
		}
	}

}

// ===================================================
void AGMCaptureTheChest::Tick(float DeltaSeconds)
{
	//Check if the first chest has to be spawned
	if (!m_SpawnedFirstChest)
	{
		//Check if all players are connected
		if (m_NumPlayersThatTravelToThisMap <= m_tpConnectedPlayerControllers.Num() && m_tpConnectedPlayerControllers.Num() > 0 && m_NumClientsWithPlayer == m_tpConnectedPlayerControllers.Num())
		{
			if (GEngine)GEngine->AddOnScreenDebugMessage(1, 100.f, FColor::Blue, FString::Printf(TEXT("GMCaptureTheChest::Tick --> All players are connected to spawn a chest. From GameMode: %i, connected controllers: %i, clients with a player: %i"), m_NumPlayersThatTravelToThisMap, m_tpConnectedPlayerControllers.Num(), m_NumClientsWithPlayer));
			
			SpawnChestRandomLoc();

			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Blue, FString::Printf(TEXT("GMCaptureTheChest::Tick --> Spawned first chest in Tick")));
		}
		else
		{
			if (GEngine)GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("GMCaptureTheChest::Tick --> Not all players are connected waiting on. From GameMode: %i, connected controllers: %i, Clients with a player: %i"), m_NumPlayersThatTravelToThisMap, m_tpConnectedPlayerControllers.Num(), m_NumClientsWithPlayer));
		}
	};

	//Calculate leftover gametime
	if (m_bGameIsOver == false)
	{
		m_RemainGameTimeInSec -= DeltaSeconds;

		if (m_RemainGameTimeInSec <= 0)
		{
			m_bGameIsOver = true;
			GetWorld()->ServerTravel("Phase2");
		}
	}
}

// ===================================================
void AGMCaptureTheChest::PostSeamlessTravel()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("GMCaptureTheChest::PostSeamlessTravel --> Succesfully travelled to another map with a new gamemode!")));
	m_NumPlayersThatTravelToThisMap = dynamic_cast<UGIPlanetaryPrivateers*>(GetGameInstance())->m_NumPlayersThatTravelToNextGameMode;

	FActorSpawnParameters spawnParams{};

	//Spawn X VR_Pawns
	for (int i = 0; i < m_NumPlayersThatTravelToThisMap; ++i)
	{
		AVRPawn* newPawn = GetWorld()->SpawnActor<AVRPawn>(m_PawnClass->StaticClass(), spawnParams);

		//If the Spawn succeeded add a reference to it
		if(newPawn)
		{
			m_tpVRPawnsOnServer.Add(newPawn);
			if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("GMCaptureTheChest::PostSeamlessTravel --> Created a new VR pawn, total: %i"), m_tpVRPawnsOnServer.Num()));
		}
		else
		{
			newPawn = GetWorld()->SpawnActor<AVRPawn>(DefaultPawnClass, spawnParams);
			if (newPawn)
			{
				m_tpVRPawnsOnServer.Add(newPawn);
				if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("GMCaptureTheChest::PostSeamlessTravel --> Created a new VR pawn, total: %i"), m_tpVRPawnsOnServer.Num()));
			}
			else
			{
				newPawn = GetWorld()->SpawnActor<AVRPawn>(DefaultPawnClass->StaticClass(), spawnParams);
				if (newPawn)
				{
					m_tpVRPawnsOnServer.Add(newPawn);
					if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("GMCaptureTheChest::PostSeamlessTravel --> Created a new VR pawn, total: %i"), m_tpVRPawnsOnServer.Num()));
				}
			}
		}
	}
}

// ===================================================
void AGMCaptureTheChest::AttachPawnToShip(APlayerController* pController) 
{
	// Attach Pawn to ship
	// 1. Attach pawn to ship
	// 2. Update APSCaptureTheChest::m_pShip

	APSCaptureTheChest* pPlayerState{ pController->GetPlayerState<APSCaptureTheChest>() };
	APawn* pPawn{ pController->GetPawn() };
	if (pPlayerState && pPawn)
	{

		// Check if ship exist
		if (!m_pShipTeam0 || !m_pShipTeam1)
		{
			// Spawn ships if not yet spawned at this point.
			SpawnShips();
		}

		// 1. Determine which ship
		int32 teamId = pPlayerState->GetTeamId();
		ABaseShip* pShipToAttachTo{ nullptr };
		// Should the player be automatically assigned if it has an invalid teamId?
		bool bValidTeamId{ teamId == 0 || teamId == 1 };
		
		// Handle players with invalid teamId
		if (!bValidTeamId)
		{
			const bool bAutoAssignTeam{ true };
			const int32 defaultTeamId{ 0 };

			if (bAutoAssignTeam && defaultTeamId >= 0)
			{
				pPlayerState->m_TeamId = defaultTeamId;
				teamId = pPlayerState->GetTeamId();
				if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("[Warning] Server: Please Fix - Connected player is not assigned to a valid team (%i). Automatically assigned to team %i. < AGMCaptureTheChest::AttachPawnToShip()"), teamId));
			}
			else
			{
				if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("[Warning] Server: Connected player is not assigned to a valid team (%i). Nothing happened. < AGMCaptureTheChest::AttachPawnToShip()"), teamId));
			}
		}

		if (teamId == 0)		pShipToAttachTo = m_pShipTeam0;
		else if (teamId == 1)	pShipToAttachTo = m_pShipTeam1;

		// 2. Attach to ship
		FAttachmentTransformRules attachRules{ EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld,false };
		pPawn->AttachToActor((AActor*)pShipToAttachTo, attachRules);

		// 3. Update player state
		pPlayerState->SetShip((AActor*)pShipToAttachTo);

		//4. Set the pawns Origin on the correct position
		pPawn->SetActorLocation(FVector(pShipToAttachTo->GetActorLocation().X, pShipToAttachTo->GetActorLocation().Y - 500.f, pShipToAttachTo->GetActorLocation().Z));

		//dynamic_cast<AVRPawn*>(pPawn)->m_pVRPawnOrigin->SetWorldLocation(FVector(pShipToAttachTo->GetActorLocation().X, pShipToAttachTo->GetActorLocation().Y - 450.f, pShipToAttachTo->GetActorLocation().Z));
		//pPawn->SetActorLocation(pShipToAttachTo->GetPlayerSpawnLocations()[pPlayerState->m_TeleporterId]->GetComponentLocation());
	}
	else
	{
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Server: Player controller does not have a playerstate on post login < PostLogin()"));
	}
}

// ===================================================
ABaseShip* AGMCaptureTheChest::GetShipOfTeam(int teamId)
{

	if (teamId == 0)		return m_pShipTeam0;
	else if (teamId == 1)	return m_pShipTeam1;
	else					return nullptr;
}

int32 AGMCaptureTheChest::GetTeamIdFromShip(ABaseShip * pShip)
{
	if (pShip == m_pShipTeam0)		return 0;
	else if (pShip == m_pShipTeam1)	return 1;
	else							return -1;
}

// ===================================================
void AGMCaptureTheChest::SpawnShips()
{
	SpawnShip(m_pShipTeam0, m_Ship0Spawn);
	SpawnShip(m_pShipTeam1, m_Ship1Spawn);
}

// ===================================================
void AGMCaptureTheChest::SpawnShip(ABaseShip *& pStoreToThisPointer, FTransform spawnTransform)
{
	// This function can be called anywhere and will spawn a ship if not yet spawned!
	// Spawn 2 ships on begin play. If a player connects and the ship hasn't spawned yet > spawn it

	if (pStoreToThisPointer)
	{
		// There already is an object. This ship has already been spawned
		return;
	}

	// Default to base class if none defined in BP
	if (!m_FlyingShipClass)
	{
		m_FlyingShipClass = ABaseShip::StaticClass();
	}

	// Spawn ship 
	FActorSpawnParameters spawnParams{};
	spawnParams.Owner = this;
	pStoreToThisPointer = GetWorld()->SpawnActor<ABaseShip>(m_FlyingShipClass, spawnTransform, spawnParams);
}

// ===================================================
void AGMCaptureTheChest::SpawnChestRandomLoc_Implementation()
{
	//Check if there are spawners registered
	if (m_tChestSpawners.Num() <= 0)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("GMCaptureTheChest::SpawnChestRandomLoc --> There are no spawners registred!")));
		return;
	}

	//Get a random spawner
	int random = FMath::RandRange(1, m_tChestSpawners.Num()) - 1;

	//Spawn the chest
	m_tChestSpawners[random]->SERVER_SpawnTreasureChest();

	if (!m_SpawnedFirstChest)
	{
		m_SpawnedFirstChest = true;
	}
}

bool AGMCaptureTheChest::SpawnChestRandomLoc_Validate()
{
	return true;
}

// ===================================================
void AGMCaptureTheChest::AddSpawner(AChestSpawner* spawner)
{
	if (spawner)
	{
		m_tChestSpawners.Add(spawner);
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Server: GMCaptureTheChest::AddSpawner --> Spawner was nullptr")));
	}
}

int AGMCaptureTheChest::GetScoreOfTeam(int teamId)
{
	return  m_tTeamScores[teamId];
}

// ===================================================
void AGMCaptureTheChest::AddScoreOfTeam(int team)
{
	// Check valid index
	bool bValidIndex{ m_tTeamScores.IsValidIndex(team) };
	if (bValidIndex)
	{
		++m_tTeamScores[team];
	
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Server: GMCaptureTheChest::AddScoreOfTeam --> Score: Team0: %d, Team1: %d"), m_tTeamScores[0], m_tTeamScores[1]));
		}
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Server: GMCaptureTheChest::AddScoreOfTeam --> Invalid team id: %d"), team));
	}
}
