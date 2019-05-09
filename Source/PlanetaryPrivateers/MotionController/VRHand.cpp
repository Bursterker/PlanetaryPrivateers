// Fill out your copyright notice in the Description page of Project Settings.

#include "VRHand.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interact/InteractAgentComponent.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// TEMP
#include "PlanetaryPrivateers/Player/VRPawn.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"

AVRHand::AVRHand()
{
	m_pInteractTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("InteractTrigger"));
	m_pInteractTrigger->SetupAttachment(m_pControllerMesh);
	m_pInteractTrigger->bHiddenInGame = false;
	m_pInteractTrigger->SetCollisionProfileName("InteractAgent");
	m_pInteractTrigger->SetGenerateOverlapEvents(true);

	m_pInteractAgent = CreateDefaultSubobject<UInteractAgentComponent>(TEXT("InteractAgent"));
	m_pInteractAgent->LinkWithPrimitive(m_pInteractTrigger);

	bReplicates = true;
}

void AVRHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (m_bShowTeleportIndicator)
	{
		FVector startLoc{ GetActorLocation() };
		FVector endLoc{startLoc + GetActorForwardVector() * m_MaxTeleportDist};
		bool bHit{ CalculateTeleportLocation(endLoc) };
		
		float indicatorLineThickness{ 1.8f };
		FColor indicatorColor{ FColor::Emerald };

		// Draw line
		DrawDebugLine(GetWorld(), startLoc, endLoc, indicatorColor, false, 0.f, (uint8)'\000', indicatorLineThickness);
		
		// Draw sphere if hit
		if (bHit)
		{
			// Draw arrow
			const FVector arrowDirection{ 0.f,0.f, 1.f };	// Point down
			const float hoverHeight{ 20.f };
			const float arrowLength{ 30.f };

			const float maxSwayDistance{ 14.f };
			const float swaySpeed{ 3.f };

			float elapsedSeconds{ UGameplayStatics::GetRealTimeSeconds(this) };
			float swayOffset{ FMath::Sin(elapsedSeconds * swaySpeed) * maxSwayDistance };
			
			FVector arrowStartLoc{ endLoc + arrowDirection * (hoverHeight + swayOffset) };
			FVector arrowEndLoc{ arrowStartLoc + arrowDirection * (arrowLength + swayOffset) };
			const float arrowSize{ 6.f };

			DrawDebugDirectionalArrow(GetWorld(), arrowEndLoc, arrowStartLoc, arrowSize, indicatorColor, false, 0.f, (uint8)'\000', indicatorLineThickness);
			
			// Draw sphere
			DrawDebugSphere(GetWorld(), endLoc, 5.f, 42, indicatorColor,false,0.f,(uint8)'\000', indicatorLineThickness);
		}
	}
}

void AVRHand::AttemptInteract()
{
	DbgInteract();
	Server_AttemptInteract();
}

void AVRHand::StopInteract()
{
	Server_StopInteract();
}

void AVRHand::SetGender(bool bFemale)
{
	if (m_pControllerMesh)
	{
		if (bFemale && m_pFemaleHandMesh)
		{
			m_pControllerMesh->SetSkeletalMesh(m_pFemaleHandMesh);
		}
		else if (!bFemale && m_pMaleHandMesh)
		{
			m_pControllerMesh->SetSkeletalMesh(m_pMaleHandMesh);
		}
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("VRHand.cpp::SetGender --> m_pControllerMesh is nullptr")));
	}
}

bool AVRHand::CalculateTeleportLocation(FVector& outLocation)
{
	// Setup parameters
	FVector startLoc{ GetActorLocation() };
	FVector	direction{ GetActorForwardVector() };
	FVector endLoc{ startLoc + direction * m_MaxTeleportDist };

	FHitResult hitResult{};
	FCollisionQueryParams queryParams{};
	queryParams.bTraceComplex = true;
	queryParams.AddIgnoredActor(this);

	// Do linetrace
	GetWorld()->LineTraceSingleByChannel(hitResult, startLoc, endLoc, ECollisionChannel::ECC_Visibility, queryParams);

	if (hitResult.bBlockingHit)
	{
		// Hit something
		// Update out reference param
		outLocation = hitResult.ImpactPoint;

		return true;
	}

	return false;
}

void AVRHand::Server_AttemptInteract_Implementation()
{
	// Find closest object with interactable interface
	DbgInteract();

	if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Server: VRHand >> Start interact"));
	bool bSuccess = m_pInteractAgent->InteractWithNearest(m_pInteractTrigger);
}

bool AVRHand::Server_AttemptInteract_Validate()
{
	return true;
}

void AVRHand::Server_StopInteract_Implementation()
{
	if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Server: VRHand >> Stop interact"));
	m_pInteractAgent->StopInteracting(true);
}

bool AVRHand::Server_StopInteract_Validate()
{
	return true;
}

void AVRHand::DbgInteract_Implementation()
{

}