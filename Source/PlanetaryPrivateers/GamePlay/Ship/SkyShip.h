// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GamePlay/Ship/BaseShip.h"
#include "SkyShip.generated.h"

class AShipSteeringWheel;
class AShipLever;
class UStaticMeshComponent;


UENUM(BlueprintType)
enum class EShipState : uint8
{
	BrokenState,		// Ship.Health < 0 -> Ship movement is disabled
	Controllable		// Ship is healthy. Default state, Ship movement is enabled 
};

/**
 * 
 */
UCLASS()
class PLANETARYPRIVATEERS_API ASkyShip : public ABaseShip
{
	GENERATED_BODY()

public:
	ASkyShip();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

protected:

	virtual void Tick(float dt) override;
	
	// Which class of actors to spawn 
	UPROPERTY(EditAnywhere, Category = "Ship Steering Settings")
	TSubclassOf<AShipSteeringWheel> m_SteeringWheelClass{};
	UPROPERTY(EditAnywhere, Category = "Ship Steering Settings")
	TSubclassOf<AShipLever> m_SpeedLeverClass{};

	// Is the blueprint readwrite needed?
	UPROPERTY(BlueprintReadWrite)	// Bllueprint read write to be able to access it via nodes in bp
	AShipSteeringWheel* m_pSteeringWheel {};
	UPROPERTY(BlueprintReadWrite)
	AShipLever* m_pSpeedLever {};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* m_pShipMesh{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship Steering Settings")
	float m_MaxForwardSpeed{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship Steering Settings")
	float m_MaxBackwardSpeed{};
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Ship Steering Settings")
	float m_MaxSteeringAngle{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship Steering Settings")
	float m_MaxElevationAngle{};

	// Should only be called from a server
	UFUNCTION(BlueprintNativeEvent)
	void Movement(float dt);

	// ==============
	// --- Health ---
//public:
//	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation)
//	void SERVER_DealDamage(float dmg);
//protected:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Health Settings")
//	float m_MaxHealth{};
//	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation)
//	void SERVER_SetHealth(float newHealth);
//	UFUNCTION(BlueprintCallable)
//	float GetCurrentHealth() { return m_CurrentHealth; }
//private:
//	UPROPERTY(Replicated)
//	float m_CurrentHealth{100.f};
//	UPROPERTY(Replicated)
//	bool m_bImmuneToDamage{ false };
	
//	// ====================
//	// ---- Ship State ----
//
//	UPROPERTY(Replicated)
//	EShipState m_ShipState = EShipState::Controllable;
//public:
//	UFUNCTION(BlueprintCallable)
//	EShipState GetShipState() { return m_ShipState; }
//protected:
//	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation)
//	void SERVER_ChangeShipState(EShipState newState);
//	UFUNCTION(BlueprintNativeEvent)	
//	void StateEntry();	// Should only be called from server
//	UFUNCTION(BlueprintNativeEvent)
//	void StateExit();	// Should ony be called from server
//	UFUNCTION(BlueprintNativeEvent)
//	void StateTickControllable(float dt);
//	UFUNCTION(BlueprintNativeEvent)
//	void StateTickBroken(float dt);
//
//	// -- Broken state settings
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Broken Settings")
//	float m_BrokenDuration{5.f};
//	FTimerHandle m_BrokenTimerHndl{};
//	UFUNCTION(BlueprintNativeEvent)
//	void OnShipBroken();

	// ========================
	// ---- Cargo Handling ----
private:
	UFUNCTION(Server, Reliable, WithValidation)
	void SERVER_RemoveCargo();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int m_ImpulseForce = 1000;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
};
