// Fill out your copyright notice in the Description page of Project Settings.

#include "VRPawn.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
//#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "Runtime/HeadMountedDisplay/Public/IXRTrackingSystem.h"	// for XRSystem(->GetHMDDevice())
#include "Runtime/HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "SteamVRChaperoneComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h" // for MinimumAreaRectangle 
#include "Runtime/Engine/Public/DrawDebugHelpers.h"	// Draw chaperone min rect (Temp - remove if dedicated mesh is added)
#include "Engine/World.h" // Spawn hand actors
#include "Kismet/GameplayStatics.h"

#include "Components/StaticMeshComponent.h"

#include "UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

#include "MotionController/VRHand.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"

#include "Components/CapsuleComponent.h"
#include "CaptureTheChest/GMCaptureTheChest.h"

// ===================================================
AVRPawn::AVRPawn()
{
	// NOTE: ADD "HeadMountedDisplay" and "SteamVR" in PrivateDependencyModuleNames in build.cs

	// NOTE: VR PERFORMANCE - Enable Instanced stereo rendering (Improvement: 15-20% CPU and 7-10% GPU)
	// Source: http://www.tomlooman.com/getting-started-with-vr/#SteamVR_Chaperone (F3 - Instanced Stereo Rendering)

	PrimaryActorTick.bCanEverTick = true;
	SetupVRComponents();

	m_pCapsuleTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Trigger"));
	m_pCapsuleTrigger->SetupAttachment(m_pCamera);

	bReplicates = true;

}

// ===================================================
void AVRPawn::SetupVRComponents()
{
	// -- Scene components --

	// VR World Origin scene cmp
	m_pVRWorldOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VRWorldOrigin"));
	RootComponent = m_pVRWorldOrigin;

	// VR Player Origin scene cmp
	m_pVRPawnOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VRPawnOrigin"));
	m_pVRPawnOrigin->SetupAttachment(m_pVRWorldOrigin);

	// Camera child of root
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_pCamera->SetupAttachment(m_pVRPawnOrigin);

	// Head mesh 
	m_pHeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Head"));
	m_pHeadMesh->SetupAttachment(m_pCamera);

	// -- Actor component -- 

	// SteamVRChaperone
		// Used to bind events to player left bound or enters bounds
		// NOTE: Chaperone is unique to Vive and SteamVr > This class "supports" oculus and psvr but won't Chaperone work for these devices.
		// Only activate Chaperone if Vive?
		// >> Option 1: Check VR device in constructor and create Chaperone if Vive
		// >> Option 2: Check VR device in BeginPlay and add Chaperone at runtime
		// >> - Will chaperone mess with things if vive is not detected? - If yes: Chaperone can't exist if psvr/oculus is supported and active. - if No: Chaperone can exist safely (maybe try to deactive at runtime?)
		// >> Option 3: Create chaperone oin constructor + Check Vr device in BeginPlay and remove/keep chaperone accordingly
	m_pChaperone = CreateDefaultSubobject<USteamVRChaperoneComponent>(TEXT("Chaperone"));
	m_pChaperone->OnLeaveBounds.AddDynamic(this, &AVRPawn::OnLeaveVRBounds);
	m_pChaperone->OnReturnToBounds.AddDynamic(this, &AVRPawn::OnReenterVRBounds);


}

// ===================================================
void AVRPawn::BeginPlay()
{
	if (HasAuthority())
	{
		Server_SpawnMotionControllers();
	}

	if (IsLocallyControlled())
	{
		SetupHMD();
	}

	//if (!HasAuthority())
	//{
	//	SERVER_TellTheGameModeIHaveSpawned();
	//}

	Super::BeginPlay();
}

// ===================================================
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Interaction Input bindings
	PlayerInputComponent->BindAction("InteractLeft", EInputEvent::IE_Pressed, this, &AVRPawn::InteractLeftHand);
	PlayerInputComponent->BindAction("InteractRight", EInputEvent::IE_Pressed, this, &AVRPawn::InteractRightHand);
	PlayerInputComponent->BindAction("InteractLeft", EInputEvent::IE_Released, this, &AVRPawn::ReleaseLeftHand);
	PlayerInputComponent->BindAction("InteractRight", EInputEvent::IE_Released, this, &AVRPawn::ReleaseRightHand);

	PlayerInputComponent->BindAxis("InteractLeft", this, &AVRPawn::InteractLeftHandAxis);
	PlayerInputComponent->BindAxis("InteractRight", this, &AVRPawn::InteractRightHandAxis);

	// Teleport Input bindings
	PlayerInputComponent->BindAction("DebugTeleportLeft", EInputEvent::IE_Pressed, this, &AVRPawn::StartTeleportLeftHand);
	PlayerInputComponent->BindAction("DebugTeleportRight", EInputEvent::IE_Pressed, this, &AVRPawn::StartTeleportRightHand);
	PlayerInputComponent->BindAction("DebugTeleportLeft", EInputEvent::IE_Released, this, &AVRPawn::ExecuteTeleportLeftHand);
	PlayerInputComponent->BindAction("DebugTeleportRight", EInputEvent::IE_Released, this, &AVRPawn::ExecuteTeleportRightHand);

	// Debug Input bindings (temp)
	PlayerInputComponent->BindAction("Button0", EInputEvent::IE_Pressed, this, &AVRPawn::InteractLeftHand);
	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &AVRPawn::InteractLeftHand);
}

// ===================================================
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1. Setup HMD if not yet setup
	if (IsLocallyControlled() && !m_bHMDIsSetup)
	{
		SetupHMD();
	}

	// 2. Replicate Camera
	// Note: None of the SceneComponents are replicated.
	ManageHMDReplication(DeltaTime);

	// Debug drawing (optional)
	if (m_pLeftHand && m_pRightHand && !IsLocallyControlled())
	{
		const FVector center{ GetActorLocation() };
		const FVector rightDir{ GetActorRightVector() };
		const float startLineOffset{ 25.f };
		const FVector leftStart{ center - rightDir * startLineOffset };
		const FVector rightStart{ center + rightDir * startLineOffset };

		DrawDebugLine(GetWorld(), leftStart, m_pLeftHand->GetActorLocation(), FColor::Blue);
		DrawDebugLine(GetWorld(), rightStart, m_pRightHand->GetActorLocation(), FColor::Red);

		DrawDebugSphere(GetWorld(), leftStart, 50, 24,FColor::Blue);

	}

	//3. Player hitbox
	//DrawDebugSphere(GetWorld(), m_CapsuleTrigger->GetComponentLocation(), 22, 24, FColor::Purple);

	//4.Tell the gamemode through clinet that this pawn is on a client
	if (!HasAuthority() && m_ToldTheGameModeIAmReady == false)
	{
		SERVER_TellTheGameModeIHaveSpawned();
		m_ToldTheGameModeIAmReady = true;
	}
}

// ===================================================
void AVRPawn::SetupHMD()
{

	// Start out false
	// Turn true if everything is setup correctly
	m_bVirtualRealityMode = false;

	// Sets correct eye height or defaults to PC-Mode if no VR device detected
	if (GEngine)
	{
		// Check if XRSystem is ok before continuing 
		TSharedPtr<IXRTrackingSystem, ESPMode::ThreadSafe> pXRSystem{ GEngine->XRSystem };

		if (pXRSystem)
		{
			IHeadMountedDisplay* pHMD{ GEngine->XRSystem->GetHMDDevice() };
			TSharedPtr<IStereoRendering, ESPMode::ThreadSafe> pStereo{ GEngine->XRSystem->GetStereoRenderingDevice() };

			bool bHMDEnabled{ pHMD->IsHMDEnabled() };
			bool bStereoEnabled{ pStereo->IsStereoEnabled() };
			
			if (!bHMDEnabled)
			{
				pHMD->EnableHMD(true);
			}
			if (!pStereo->IsStereoEnabled())
			{
				pStereo->EnableStereo(true);
			}

			bool bInVR{ pHMD != nullptr && pHMD->IsHMDEnabled() && pStereo->IsStereoEnabled() };
			if (bInVR)
			{
				//if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AVRPawn::AVRPawn() > XRSystem detected HMD. Pawn set to VR-Mode"));
				// Virtual reality mode enabled
				m_bVirtualRealityMode = true;

				// Set Eye height depending on Vive, Oculus, PSVR (Most likely Vive)
				{
					FName vrDeviceName = GEngine->XRSystem->GetSystemName();
					const FName oculusName{ "OculusHMD" };
					const FName	viveName{ "SteamVR" };
					const FName psvrName{ "PSVR" };

					if (vrDeviceName == viveName || vrDeviceName == oculusName)
					{
						// VROrigin is at floor level
						UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
						if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("HMD set to floor"));
					}
					else if (vrDeviceName == psvrName)
					{
						// VROrigin is at eye level
						UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);

						// Match VROrigin to eye level
						const float defaultPlayerHeight{ 180.f };
						FVector psvrEyeOffset{ 0.f,0.f,defaultPlayerHeight };
						m_pVRPawnOrigin->AddLocalOffset(psvrEyeOffset);
					}
				}
				m_bHMDIsSetup = true;

			}
			else
			{
				if (GEngine)GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::White, TEXT("AVRPawn::AVRPawn() > XRSystem did not find HMD. Pawn set to PC-Mode"));
				// Add SetupPCComponents?
			}
		}
		else
		{
			if (GEngine)GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::White, TEXT("AVRPawn::AVRPawn() > XRSystem was nullptr. Pawn set to PC-Mode"));

		}
	}
	else
	{
		// Note - May be removed - If ue4 auto possesses HMD, then it is possible that GEngine == nullptr AND player will use HMD >>> PC-Mode wrongfully selected.
		if (GEngine)GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Red, TEXT("[ERROR] GEngine was nullptr. Fix this << AVRPawn::SetupHMD()"));
	}
}

#pragma region VRBounds
// ===================================================
void AVRPawn::DrawVRBounds()
{
	// TODO: Replace DbgLine draw with dedicated vr bounding box mesh + material (visible only when close)

	// Visualize Chaperone soft bounds (irl walls)
	// See: https://api.unrealengine.com/INT/API/Plugins/SteamVR/USteamVRChaperoneComponent/index.html for GetBouns() info
	//TArray<FVector> tBoundVerts{ m_pChaperone->GetBounds() };
	//FVector surfaceNormal{ m_pVRPawnOrigin->GetUpVector() }; // Assumes player is standing on flat surface pointing up (for the player)
	//FVector outRectCenter;
	//FRotator outRectRotation;
	//float outSideLengthX;
	//float outSideLengthY;
	//UKismetMathLibrary::MinimumAreaRectangle(this, tBoundVerts, surfaceNormal, outRectCenter, outRectRotation, outSideLengthX, outSideLengthY);
	//
	//// Remove if dedicated mesh is created (also remove #include "dbgDraw...")
	//float lineThickness{ 5.f };
	//size_t nbVerts{ (size_t)tBoundVerts.Num() };
	//FColor lineColor{ FColor::Cyan };
	//for (size_t i = 0; i < nbVerts; i++)
	//{
	//	size_t lineStartIdx{ i };
	//	size_t lineEndIdx{ (i + 1) % nbVerts };
	//	DrawDebugLine(GetWorld(), tBoundVerts[lineStartIdx], tBoundVerts[lineEndIdx], lineColor, true, 300000.f, (uint8)'\000', lineThickness);
	//}
}

// ===================================================
void AVRPawn::OnLeaveVRBounds()
{
	if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("[AVRPawn] Player has left VR bounds"));
	// Use this in the future
}

// ===================================================
void AVRPawn::OnReenterVRBounds()
{
	if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("[AVRPawn] Player has reentered VR bounds"));
	// Use this in the future

}
#pragma endregion VRBounds

// SERVER
// ===================================================
void AVRPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVRPawn, m_pLeftHand);
	DOREPLIFETIME(AVRPawn, m_pRightHand);
	DOREPLIFETIME(AVRPawn, m_pCapsuleTrigger);
}

// ===================================================
void AVRPawn::ToggleVR()
{
	// 1. Try to disable vr tracking
	TSharedPtr<IXRTrackingSystem, ESPMode::ThreadSafe> pXRSystem{ GEngine->XRSystem };
	if (pXRSystem)
	{
		IHeadMountedDisplay* pHMD{ GEngine->XRSystem->GetHMDDevice() };
		bool bHMDEnabled{ pHMD->IsHMDEnabled() };
		pHMD->EnableHMD(!pHMD->IsHMDEnabled());
		if (bHMDEnabled)
		{
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, ("HMD WAS ON"));
		}
		else
		{
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, ("HMD WAS OFF"));
		}

	}
}

// ===================================================
void AVRPawn::ToggleStereo()
{
	// 1. Try to disable vr tracking
	TSharedPtr<IXRTrackingSystem, ESPMode::ThreadSafe> pXRSystem{ GEngine->XRSystem };
	if (pXRSystem)
	{
		TSharedPtr<IStereoRendering, ESPMode::ThreadSafe> pStereo{ pXRSystem->GetStereoRenderingDevice() };
		pStereo->EnableStereo(!pStereo->IsStereoEnabled());
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("Toggle Stereo VR"));
	}

}

// ===================================================
void AVRPawn::Server_SpawnMotionControllers_Implementation()
{
	// Called on every pawn on the server
	// Server spawns motion control actors
	// Clients > OnRep_LeftRightHand()
	UWorld* pWorld{ GetWorld() };
	FTransform spawnTransform{};
	spawnTransform.SetLocation(m_pCamera->GetComponentLocation() + FVector(0.f, 0.f, 200.f));
	spawnTransform.SetRotation(FQuat(90, 0, 0, 0));

	if (!m_pVRControllerClass)
	{
		m_pVRControllerClass = AVRHand::StaticClass();
	}

	// 1. Spawn hands
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	m_pLeftHand = pWorld->SpawnActor<AVRHand>(m_pVRControllerClass, spawnTransform, spawnParams);
	
	//spawnTransform.SetScale3D(FVector(1.f, -1.f, 1.f));
	m_pRightHand = pWorld->SpawnActor<AVRHand>(m_pVRControllerClass, spawnTransform, spawnParams);

	// 3. Attach Hands to Origin(Server only, Clients do this with OnRep...)
	InitializeNewMotionController(m_pLeftHand, EControllerHand::Left);
	InitializeNewMotionController(m_pRightHand, EControllerHand::Right);
}

bool AVRPawn::Server_SpawnMotionControllers_Validate()
{
	return true;
}

void AVRPawn::SpawnMotionCOntrolllers()
{
	// Called on every pawn on the server
	// Server spawns motion control actors
	// Clients > OnRep_LeftRightHand()
	UWorld* pWorld{ GetWorld() };
	FTransform spawnTransform{};
	spawnTransform.SetLocation(m_pCamera->GetComponentLocation() + FVector(0.f, 0.f, 200.f));
	FActorSpawnParameters spawnParams;

	if (!m_pVRControllerClass)
	{
		m_pVRControllerClass = AVRHand::StaticClass();
	}

	//if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 235.f, FColor::Emerald, FString::Printf(TEXT("[Server] Spawning controllers for: %s"), *GetHumanReadableName()));

	// 1. Spawn left hand
	//spawnParams.Name = "LeftHand"; // maybe change to LeftHand_PLAYERID later
	spawnParams.Owner = this;
	m_pLeftHand = pWorld->SpawnActor<AVRHand>(m_pVRControllerClass, spawnTransform, spawnParams);
	//m_pLeftHand->SetTrackingHand(EControllerHand::Left);

	// 2. Spawn right hand
	//spawnParams.Name = "RightHand"; // maybe change to LeftHand_PLAYERID later
	//m_pRightHand = pWorld->SpawnActor<AVRHand>(m_pVRControllerClass, spawnTransform, spawnParams);
	//m_pRightHand->SetTrackingHand(EControllerHand::Right);

	// 3. Attach Hands to Origin(Server only, Clients do this with OnRep...)
	InitializeNewMotionController(m_pLeftHand, EControllerHand::Left);
	//InitializeNewMotionController(m_pRightHand, EControllerHand::Right);
}

void AVRPawn::OnRep_LeftHandController()
{
	InitializeNewMotionController(m_pLeftHand, EControllerHand::Left);
}

void AVRPawn::OnRep_RightHandController()
{
	InitializeNewMotionController(m_pRightHand, EControllerHand::Right);
}

// ===================================================
void AVRPawn::InitializeNewMotionController(AVRHand* pVRHand,  EControllerHand trackedHand)
{
	if (!pVRHand)
	{
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("VRPawn could not initilaize motion controller. pController was nullptr. Exited function. << AVRPawn::InitializeNewMotionController()"));
		return;
	}

	// 1. Attach AMotionController to this pawn
	FAttachmentTransformRules attachRules{ EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false };
	pVRHand->AttachToComponent(m_pVRPawnOrigin, attachRules);

	// 2. Enable tracking if owned by this client + set Gender
	bool bFemale{ true };
	
	FString instanceTag{ GetNetMode() == NM_DedicatedServer ? "[Server]" : "[Client]"};
	
	bool bIsServer{ GetNetMode() == NM_DedicatedServer };

	if (!bIsServer && IsLocallyControlled())
	{
		// 2. Set motion source
		pVRHand->SetTrackingHand(trackedHand);
		// Female
		pVRHand->SetTrackingMode(ETrackingMode::Tracked);
		pVRHand->SetGender(bFemale);

		//if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("%s Pawn is locally controlled > Female hands"), *instanceTag));
	}
	else
	{
		// Male 
		bFemale = false;
		pVRHand->SetGender(bFemale);
		pVRHand->SetTrackingHand(trackedHand);
		pVRHand->SetTrackingMode(ETrackingMode::Replicated);

		//if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("%s Pawn is NOT locally controlled > Male hands"), *instanceTag));
	}
	
}

// ===================================================
void AVRPawn::InteractLeftHand()
{
	if (m_pLeftHand)
	{
		m_pLeftHand->AttemptInteract();
	}
}

// ===================================================
void AVRPawn::InteractRightHand()
{

	if (m_pRightHand)
	{
		m_pRightHand->AttemptInteract();
	}
}

 //===================================================
void AVRPawn::ReleaseLeftHand()
{
	if (m_pLeftHand)
	{
		m_pLeftHand->StopInteract();
	}
}

// ===================================================
void AVRPawn::ReleaseRightHand()
{
	if (m_pRightHand)
	{
		m_pRightHand->StopInteract();
	}
}

// ===================================================
void AVRPawn::StartTeleportRightHand()
{
	if (m_bAllowTeleporting && m_pRightHand)
	{
		m_pRightHand->EnableTeleportIndicator(true);
	}
}

// ===================================================
void AVRPawn::StartTeleportLeftHand()
{
	if (m_bAllowTeleporting && m_pLeftHand)
	{
		m_pLeftHand->EnableTeleportIndicator(true);
	}
}

// ===================================================
void AVRPawn::InteractLeftHandAxis(float AxisValue)
{
	
}

// ===================================================
void AVRPawn::InteractRightHandAxis(float AxisValue)
{
	
}

// ===================================================
void AVRPawn::ExecuteTeleportRightHand()
{
	// Calculate position on Client side instead of serverside 
	// Location should be relative to m_pVRPawnOrigin?

	ExecuteTeleportFromHand(m_pRightHand);
}

// ===================================================
void AVRPawn::ExecuteTeleportLeftHand()
{
	ExecuteTeleportFromHand(m_pLeftHand);
}

// ===================================================
void AVRPawn::ExecuteTeleportFromHand(AVRHand * pHand)
{
	if (m_bAllowTeleporting && pHand)
	{
		pHand->EnableTeleportIndicator(false);
		FVector teleportTaretLocation{};
		bool bCanTeleport{ pHand->CalculateTeleportLocation(teleportTaretLocation) };

		if (bCanTeleport)
		{

			// Start teleport location from HMD location
			FVector teleportStartLocation{ m_pCamera->GetComponentLocation() };
			teleportStartLocation.Z = m_pVRPawnOrigin->GetComponentLocation().Z;	// Set height to ground level
			
			//DrawDebugSphere(GetWorld(), teleportTaretLocation, 20.f, 12, FColor::Blue, true, 335.f);
			//DrawDebugSphere(GetWorld(), teleportStartLocation, 20.f, 12, FColor::Red, true, 335.f);



			// Teleport offset to add relatively to m_pVRPawnOrigin
			FVector teleportDelta{ teleportTaretLocation - teleportStartLocation};



			Server_TeleportToLocation(teleportDelta);
		}
	}

}

// ===================================================
void AVRPawn::Server_TeleportToLocation_Implementation(FVector teleportLocation)
{
	MC_TeleportToLocation(teleportLocation);	// Clients teleport
}

bool AVRPawn::Server_TeleportToLocation_Validate(FVector teleportLocation)
{
	return true;
}

// ===================================================
void AVRPawn::MC_TeleportToLocation_Implementation(FVector teleportLoc)
{
	TeleportToLocation(teleportLoc);
}

bool AVRPawn::MC_TeleportToLocation_Validate(FVector teleportLoc)
{
	return true;
}

// ===================================================
void AVRPawn::TeleportToLocation(FVector teleportLoc)
{
	//FVector beforeTeleLoc{ m_pVRPawnOrigin->GetComponentLocation() };

	// Teleport location is relative to m_pVRPawnOrigin
	m_pVRPawnOrigin->AddWorldOffset(teleportLoc);

	//FVector afterTeleLoc{ m_pVRPawnOrigin->GetComponentLocation() };
	//DrawDebugSphere(GetWorld(), beforeTeleLoc, 20.f, 24, FColor::Black, false, 20.f);
	//DrawDebugSphere(GetWorld(), afterTeleLoc, 20.f, 24, FColor::White, false, 20.f);
	//DrawDebugDirectionalArrow(GetWorld(), beforeTeleLoc, afterTeleLoc, 5.f, FColor::White, true, 20.f);
}



// =================================================== 
// -- Camera transform replication to all not-owning instance --
#pragma region HMD_Replication
void AVRPawn::ManageHMDReplication(float dt)
{
	if (IsLocallyControlled())
	{
		// Owning client 
		// Should tell other instance to copy RELATIVE transform to m_pVRPawnOrigin
		Server_RepCameraTransformToAll(m_pCamera->GetRelativeTransform());
	}
	else 
	{
		if (m_bShouldLerpRepHMDTransform)	// only True if Multicast_RepCameraTransform() is called. Will be set to false automatically
		{
			// Non-owning client/server
			// Lerp transform and stop lerping to rep transform if close enough

			// 1.Lerp transform to replicated value
			const float lerpSpeed{ 10.f };
			float alpha = dt * lerpSpeed;

			const FTransform currentWorldTrans{ m_pCamera->GetComponentTransform() };
			const FTransform parentTransform{ m_pVRPawnOrigin->GetComponentTransform() };
			const FTransform targetWorldTrans{ m_HMDRepRelTransTarget * parentTransform }; // Relative (to m_pVRPawnOrigin) space to world space

			// New lerped transform in world space
			FVector lerpedLocation{ FMath::Lerp(currentWorldTrans.GetLocation(), targetWorldTrans.GetLocation(), alpha) };
			FQuat lerpedRotation{ FMath::Lerp(currentWorldTrans.GetRotation(), targetWorldTrans.GetRotation(), alpha) };
			FVector lerpedScale{ FMath::Lerp(currentWorldTrans.GetScale3D(),targetWorldTrans.GetScale3D(),alpha) };

			m_pCamera->SetWorldTransform(FTransform(lerpedRotation, lerpedLocation, lerpedScale));
			return;
			// 2. Check if should still lerp:
			const FTransform lerpedTransform{ m_pCamera->GetComponentTransform() };

			const float locTolernace{ 0.2f };	// Not squared
			bool bLocationMatches{ FTransform::AreTranslationsEqual(lerpedTransform, targetWorldTrans,locTolernace) };

			const float rotTolerance{ 0.2f };
			bool bRotMatches{ FTransform::AreRotationsEqual(lerpedTransform,targetWorldTrans,rotTolerance) };

			const float scaleTolerance{ 0.05f };
			bool bScaleMatches{ FTransform::AreScale3DsEqual(lerpedTransform,targetWorldTrans,scaleTolerance) };

			if (bLocationMatches && bScaleMatches && bRotMatches)
			{
				m_bShouldLerpRepHMDTransform = false;
			}
		}
	}
}

void AVRPawn::AlignCameraUpWithGravityUp()
{
	// Won't work - cannont change hmd transform directly
	// Needs to change source code for this to work

	// Below code is correct, but the hmd transform is done during the render stage.
	// Changing camera transform won't work, even if tickgroup is PostWork(last possible group)
	return;

	// 1. Move camera to feet location (tracking is relative to actor transform > follow the actor up vector)
	FVector vrDown { -m_pVRWorldOrigin->GetUpVector() };
	const float playerHeight{ 180.f };
	m_pCamera->AddWorldOffset(vrDown * playerHeight);

	// 2. Do rotation correction
	float pitchCorrection{ -m_pVRWorldOrigin->GetComponentRotation().Pitch };
	m_pCamera->AddWorldRotation(FQuat(FRotator(pitchCorrection, 0.f,0.f)));

	// 3. Move to correct location (from feet follow gravity up direction)
	m_pCamera->AddWorldOffset(FVector::UpVector * playerHeight);
}

// =================================================== 
void AVRPawn::SERVER_TellTheGameModeIHaveSpawned_Implementation()
{
	AGMCaptureTheChest* gameMode = dynamic_cast<AGMCaptureTheChest*>(GetWorld()->GetAuthGameMode());

	if (gameMode)
	{
		gameMode->m_NumClientsWithPlayer++;
		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Blue, FString::Printf(TEXT("VRPawn::SERVER_TellTheGameModeIHaveSpawned_Implementation --> incremented the gamemode Clients with player!")));
	}
}
bool AVRPawn::SERVER_TellTheGameModeIHaveSpawned_Validate()
{
	return true;
}
// =================================================== 
void AVRPawn::Server_RepCameraTransformToAll_Implementation(FTransform cameraRelativeTransform)
{
	Multicast_RepCameraTransform(cameraRelativeTransform);
}
// =================================================== 
bool AVRPawn::Server_RepCameraTransformToAll_Validate(FTransform cameraRelativeTransform)
{
	return true;
}

// =================================================== 
void AVRPawn::Multicast_RepCameraTransform_Implementation(FTransform cameraRelativeTransform)
{
	if (!IsLocallyControlled())
	{
		// If not owning client 
		m_HMDRepRelTransTarget = cameraRelativeTransform;
		m_bShouldLerpRepHMDTransform = true;
	}
}
// =================================================== 
bool AVRPawn::Multicast_RepCameraTransform_Validate(FTransform cameraRelativeTransform)
{
	return true;
}
#pragma endregion HMD_Replication