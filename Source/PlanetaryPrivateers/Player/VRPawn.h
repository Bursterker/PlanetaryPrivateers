// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

class UCameraComponent;
class USteamVRChaperoneComponent;
class AMotionControllerHand;

class AVRHand;
class AVRMotionController;
class UCapsuleComponent;

/*
VR PAWN
- Head tracking on client owned pawn
- HMD transform is replicated for all other pawns

--- Motion controllers ---
- Client owned pawn spawns 2 motion controllers at begin play
- Motion controllers are replicated
- Motion controllers are only activly tracked on the owning client
- Motion controllers transform is replicated to non-owning clients and server

*/


UCLASS()
class PLANETARYPRIVATEERS_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void DrawVRBounds();
	bool m_bVirtualRealityMode = false; // True if HMD is detected.
	// do vr mode first > than pc mode

private:
	// Init default components
	void SetupVRComponents();
	// Moves with ship
	UPROPERTY()
	USceneComponent* m_pVRWorldOrigin;
public:
	// Moves with height on ship (if needed)
	UPROPERTY(BlueprintReadOnly)
	USceneComponent* m_pVRPawnOrigin;
private:
	UPROPERTY()
	UCameraComponent* m_pCamera;
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_pHeadMesh;


	// VR setup
	UFUNCTION()
	void SetupHMD();
	bool m_bHMDIsSetup{ false };


	// ============================
	// ---- Steam VR Chaperone ----

	UFUNCTION()
	void OnLeaveVRBounds();
	UFUNCTION()
	void OnReenterVRBounds();

	UPROPERTY()
	USteamVRChaperoneComponent* m_pChaperone;


	// =================================
	// ---- Hands/MotionControllers ----
protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LeftHandController)
	AVRHand* m_pLeftHand;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RightHandController)
	AVRHand* m_pRightHand;
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AVRHand> m_pVRControllerClass;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SpawnMotionControllers();
	void SpawnMotionCOntrolllers();
	void InitializeNewMotionController(AVRHand* pVRHand, EControllerHand trackedHand);

	UFUNCTION()
	void OnRep_LeftHandController();
	UFUNCTION()
	void OnRep_RightHandController();

	UFUNCTION()
	void InteractLeftHand();
	UFUNCTION()
	void InteractRightHand();
	UFUNCTION()
	void ReleaseLeftHand();
	UFUNCTION()
	void ReleaseRightHand();
	UFUNCTION()
		void InteractLeftHandAxis(float AxisValue);
	UFUNCTION()
		void InteractRightHandAxis(float AxisValue);
public:
	UFUNCTION(BlueprintCallable)
	AVRHand* GetLeftHand() { return m_pLeftHand; }
	UFUNCTION(BlueprintCallable)
	AVRHand* GetRightHand() { return m_pRightHand; }
private:
	// ========================
	// ---- Debug Teleport ----
	UFUNCTION()
	void StartTeleportRightHand();
	UFUNCTION()
	void StartTeleportLeftHand();
	UFUNCTION()
	void ExecuteTeleportRightHand();
	UFUNCTION()
	void ExecuteTeleportLeftHand();
	UFUNCTION()
	void ExecuteTeleportFromHand(AVRHand* pHand);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_TeleportToLocation(FVector teleportLocation);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MC_TeleportToLocation(FVector teleportLoc);
	void TeleportToLocation(FVector teleportLoc);

	protected:
	UPROPERTY(EditAnywhere)
	bool m_bAllowTeleporting{ true };

	UFUNCTION()
	void ToggleVR();
	UFUNCTION()
	void ToggleStereo();

	// =====================
	// ---- Replication ----
	// For replicated properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
	
	// ---- HMD Replication ----

	UFUNCTION(Server, Unreliable, WithValidation)
		void Server_RepCameraTransformToAll(FTransform cameraTransform);	// Owning client to Server
	// I don't think validation is needed for NetMultiCast from server
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void Multicast_RepCameraTransform(FTransform cameraTransform);	// Server to all instances

	// Replicate Transform
	void ManageHMDReplication(float dt);	// Both owning client and others
	FTransform m_HMDRepRelTransTarget;		// HMD relative transform target (non-owning clients/server only)
	bool m_bShouldLerpRepHMDTransform{ false };


	// Make sure players perceived up is always gravity its up vector (0,0,1)
	//UFUNCTION(BlueprintNativeEvent)
	void AlignCameraUpWithGravityUp();

	public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* m_pCapsuleTrigger = nullptr;

	//---- ReadyCheckFromClient ----
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_TellTheGameModeIHaveSpawned();

	bool m_ToldTheGameModeIAmReady = false;
};