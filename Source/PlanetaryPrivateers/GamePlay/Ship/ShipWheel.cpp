// Fill out your copyright notice in the Description page of Project Settings.

#include "ShipWheel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"
#include "Interact/InteractableComponent.h"
#include "Interact/InteractAgentComponent.h"
#include "Engine/Engine.h"
#include "UnrealNetwork.h"

#include "DrawDebugHelpers.h"

// ===================================================
AShipWheel::AShipWheel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. Root scene component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 2. Base mesh
	m_pSMBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMBase"));
	m_pSMBase->SetupAttachment(RootComponent);
	m_pSMBase->SetSimulatePhysics(false);	// Kinematic body
	m_pSMBase->SetCollisionProfileName("BlockAll");

	// 3. Wheel Body collider
	m_pWheelAxisBody = CreateDefaultSubobject<UBoxComponent>(TEXT("WheelAxisBody"));
	m_pWheelAxisBody->SetupAttachment(RootComponent);
	m_pWheelAxisBody->SetBoxExtent(FVector(5.f));
	m_pWheelAxisBody->SetCollisionProfileName("PhysicsConstraintNoCol");	// Collision: Query only, Ignore all collision
	m_pWheelAxisBody->SetSimulatePhysics(true);

	// 4. Wheel axis physical constraint
	m_pWheelAxisConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("WheelAxisConstraint"));
	m_pWheelAxisConstraint->SetupAttachment(RootComponent);
	// Angular limits
	// NOTE: This is override by blueprint - for [-1,1] values: make sure both m_WheelAngularLimit corresponds to that assigned in the physicsconstraint Angular Limits tab!
	m_pWheelAxisConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, m_WheelAngularLimit); 

	// 5. Wheel static mesh
	m_pSMWheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMWheel"));
	m_pSMWheel->SetupAttachment(m_pWheelAxisBody);
	m_pSMWheel->SetCollisionProfileName("Interactable");

	// 6. Grab physics constraint - Does nothing while wheel is not grabbed
	m_pGrabConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("GrabConstraint"));
	m_pGrabConstraint->SetupAttachment(RootComponent);
	// - Default Linear constraints - MODIFY THIS IN BLUEPRINTS
	m_pGrabConstraint->SetLinearXLimit(LCM_Locked,0.f);
	m_pGrabConstraint->SetLinearYLimit(LCM_Locked, 0.f);
	m_pGrabConstraint->SetLinearZLimit(LCM_Locked, 0.f);
	m_pGrabConstraint->SetLinearPositionTarget(FVector::ZeroVector);
	m_pGrabConstraint->SetLinearPositionDrive(true, true, true);
	const float linearPosDrive{ 1000.f };
	const float linearVelDrive{ 100.f };
	const float linearForceLimit{ 0.f };
	m_pGrabConstraint->SetLinearDriveParams(linearPosDrive, linearVelDrive, linearForceLimit);

	// - Default Angular constraints - MODIFY THIS IN BLUEPRINTS
	m_pGrabConstraint->SetAngularSwing1Limit(ACM_Free, 0.f);
	m_pGrabConstraint->SetAngularSwing2Limit(ACM_Free, 0.f);
	m_pGrabConstraint->SetAngularTwistLimit(ACM_Free, 0.f);
	m_pGrabConstraint->SetAngularVelocityTarget(FVector::ZeroVector);
	m_pGrabConstraint->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
	m_pGrabConstraint->SetAngularVelocityDriveTwistAndSwing(true, true);
	const float angularPosDrive{ 500.f };
	const float angularVelDrive{ 100.f };
	const float angularForceLimit{ 0.f };
	m_pGrabConstraint->SetAngularDriveParams(angularPosDrive, angularVelDrive, angularForceLimit);

	// 7. Interactable component wheel grab
	m_pGrabInteractable = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	m_pGrabInteractable->LinkWithCollider(m_pSMWheel);
	m_pGrabInteractable->OnInteractStartEvent.AddDynamic(this, &AShipWheel::GrabInteractStart);
	m_pGrabInteractable->OnInteractStopEvent.AddDynamic(this, &AShipWheel::GrabInteractStop);

	bReplicates = true;
}

// ===================================================
void AShipWheel::BeginPlay()
{
	Super::BeginPlay();

	// 1. Disable Tick function on remote instance
	if (!HasAuthority())
	{
		//PrimaryActorTick.SetTickFunctionEnable(false);
	}

	// 2. 
	//SetupConstraintComponents();

}

// ===================================================
void AShipWheel::Tick(float DeltaTime)
{
	// NOTE: Tick is disabled on remote clients

	Super::Tick(DeltaTime);

	// Only server caluclates
	if (HasAuthority())
	{
		CalculateWheelAxis();
	}
}

// ===================================================
void AShipWheel::SetupConstraintComponents()
{
	// -- Constraint settings -- 
	// Components to link
	m_pWheelAxisConstraint->SetConstrainedComponents(m_pSMBase, NAME_None, m_pWheelAxisBody, NAME_None);
	
	m_pWheelAxisConstraint->SetLinearXLimit(LCM_Locked, 0.f);
	m_pWheelAxisConstraint->SetLinearYLimit(LCM_Locked, 0.f);
	m_pWheelAxisConstraint->SetLinearZLimit(LCM_Locked, 0.f);

	// - Default Angular constraints - MODIFY THIS IN BLUEPRINTS
	m_pWheelAxisConstraint->SetAngularSwing1Limit(ACM_Locked, 0.f);
	m_pWheelAxisConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, m_WheelAngularLimit);
	m_pWheelAxisConstraint->SetAngularTwistLimit(ACM_Locked, 0.f);
	
}

void AShipWheel::CalculateWheelAxis()
{
	// Note: no reason to call this on remote clients
	// - Won't be synced perfectly with server
	// - Server only instance that should use m_WheelAxis (for ship rotation)
	const float currentRot{ m_pWheelAxisConstraint->GetCurrentSwing2() };
	//if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString::Printf(TEXT("Server: Swing2: %f"), currentRot));
	m_WheelAxis = currentRot / m_WheelAngularLimit;
}

// ===================================================
void AShipWheel::GrabInteractStart(UInteractableComponent* pInteractableCmp, UInteractAgentComponent* pAgentCmp)
{
	// Server only
	UPrimitiveComponent* pAgentPrimitive{ pAgentCmp->GetPrimitiveAgent() };
	if (!pAgentPrimitive)
	{
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Server: Agent did not have a linked primitive. < AShipWheel::GrabInteractStart()"));
	}

	// Tell all instances
	MC_SetupGrabConstraint(pAgentPrimitive);
}

// ===================================================
void AShipWheel::GrabInteractStop()
{
	MC_ReleaseGrabConstraint();
}

// ===================================================
void AShipWheel::MC_SetupGrabConstraint_Implementation(UPrimitiveComponent * pGrabbedPrim)
{
	// 1. Check if valid input
	bool bValidPrim{ pGrabbedPrim != nullptr };
	m_bGrabbed = bValidPrim;

	// 2. Setup physical constriant - Only if valid input
	if (m_bGrabbed)
	{
		m_pGrabConstraint->SetWorldLocation(pGrabbedPrim->GetComponentLocation());
		FAttachmentTransformRules attachRules{ FAttachmentTransformRules::SnapToTargetNotIncludingScale };
		attachRules.bWeldSimulatedBodies = true;
		m_pGrabConstraint->AttachToComponent(pGrabbedPrim, attachRules);
		m_pGrabConstraint->SetConstrainedComponents(pGrabbedPrim, NAME_None, m_pWheelAxisBody, NAME_None);
	}

}

// ===================================================
bool AShipWheel::MC_SetupGrabConstraint_Validate(UPrimitiveComponent * pGrabbedPrim)
{
	return true;
}

// ===================================================
void AShipWheel::MC_ReleaseGrabConstraint_Implementation()
{
	m_bGrabbed = false;
	m_pGrabConstraint->BreakConstraint();
	m_pGrabConstraint->AttachToComponent(RootComponent, { FAttachmentTransformRules::SnapToTargetNotIncludingScale });
}

// ===================================================
bool AShipWheel::MC_ReleaseGrabConstraint_Validate()
{
	return true;
}

// ===================================================
void AShipWheel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShipWheel, m_WheelAxis);

}