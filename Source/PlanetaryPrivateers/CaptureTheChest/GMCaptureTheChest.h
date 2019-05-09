// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/GMPlanetaryPrivateers.h"
#include "TreasureChest.h"
#include "GMCaptureTheChest.generated.h"

class AChestSpawner;
class APawn;
class AVRPlayerController;
class ABaseShip;
class AVRPawn;

UCLASS()
class PLANETARYPRIVATEERS_API AGMCaptureTheChest : public AGMPlanetaryPrivateers
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/**Called if this gamemode is created after a seamless travel transition*/
	virtual void PostSeamlessTravel() override;

	/**
 * ENTRY POINT: Player connects to new level (pawn and playercontroller are valid)
 * Function called when a player successfuly loads a new map.
 * Works with both travel methods
 * - Non-Seamless travel: called from PostLogin()
 * - Seamless trave: called from HandleSeamlessTravelPlayer()
 */
	virtual void GenericPlayerInitialization(AController* pController) override;

	// ======================================
	// --- Reference to connected players ---
	virtual void PostLogin(APlayerController* NewPlayer) override;
private:
	/**Function that keeps a reference to the connected player controller*/
	void KeepReferenceToPlayer(APlayerController* pController);

	TArray<APlayerController*> m_tpConnectedPlayerControllers;

	int32 m_NumPlayersThatTravelToThisMap = 0;
public:
	int32 m_NumClientsWithPlayer = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AVRPawn> m_PawnClass;

	TArray<AVRPawn*> m_tpVRPawnsOnServer;
	int m_NextUnassignedPawn = 0;

	// ===========================
	// --- Attach pawn to ship ---
	// Assign this player to a ship
private:
	// For Debugging purposes - normal game should have this turned on probably
	UPROPERTY(EditAnywhere, Category = "Debug Settings")
	bool m_bAutoAttachToShip{ true };

	/**
	 * Function will attach pawn to ship if pawn is valid
	 * If Pawn invalid - add delegate that gets called when player controller starts possessing a new pawn
	 */
	void TryAttachPawnToShip(AVRPlayerController* pVRController);

	/**
	 * Function that attaches a player controller's pawn to the correct ship
	 * Requires:
	 * 1. AVRPlayerController (PlayerController)
	 * 2. APSCaptureTheChest  (PlayerState)
	 *
	 * Can be called before team ships are spawned
	 * > Will spawn the required ship if not yet spawned
	 */
	UFUNCTION()
	void AttachPawnToShip(APlayerController* pController);

private:
	// ---- Ships -----
	UPROPERTY(EditAnywhere, Category = "Ship", meta = (ToolTip = "Which Flying Ship should be spawned"))
	TSubclassOf<ABaseShip> m_FlyingShipClass; 

	void SpawnShips();
	void SpawnShip(ABaseShip*& pStoreToThisPointer, FTransform spawnTransform);

	UPROPERTY()
	ABaseShip* m_pShipTeam0;
	UPROPERTY()
	ABaseShip* m_pShipTeam1;

protected:
	UPROPERTY(EditAnywhere, Category = "Ship", meta = (ToolTip = "Transform where the ship should spawn"))
	FTransform m_Ship0Spawn;
	UPROPERTY(EditAnywhere, Category = "Ship", meta = (ToolTip = "Transform where the ship should spawn"))
	FTransform m_Ship1Spawn;

public:
	UFUNCTION(BlueprintCallable)
	ABaseShip* GetShipOfTeam(int teamId);
	UFUNCTION(BlueprintCallable)
	int32 GetTeamIdFromShip(ABaseShip* pShip);

public:
	// ---- Treasure Chest ----
	UFUNCTION(Server, Reliable, WithValidation)
		void SpawnChestRandomLoc();

	void AddSpawner(AChestSpawner* spawner);

private:
	TArray<AChestSpawner*> m_tChestSpawners;
	bool m_SpawnedFirstChest = false;

	UPROPERTY(EditAnywhere, Category = "Treasure Chest Settings")
	TSubclassOf<ATreasureChest> m_pTreasureChestClass;
	
	// Current treasure chest spawned on map
	UPROPERTY()
	ATreasureChest* m_pTreasureChest;

	UPROPERTY(EditAnywhere, Category = "Map Settings")
	FVector m_MapCenter;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
	float m_MapRadius{40000.f};


	//---- Team ----
	public:
		UFUNCTION(BlueprintCallable)
		int GetScoreOfTeam(int teamId);

private:
	TArray<int>m_tTeamScores = { 0,0 };

public:
	void AddScoreOfTeam(int team);

	//---- GameEnd ----
public:
	/**Returns wether the game has ended or not*/
	bool CheckIsGameOver() { return m_bGameIsOver; }

	/**Returns the remaining gameTime in seconds*/
	int GetRemainingGameTime() { return m_RemainGameTimeInSec; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int m_TotalGameTimeInSec = 600;

private:
	float m_RemainGameTimeInSec = 0;
	bool m_bGameIsOver = false;
};
