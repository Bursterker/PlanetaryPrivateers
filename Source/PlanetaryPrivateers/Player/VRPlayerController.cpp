// Fill out your copyright notice in the Description page of Project Settings.

#include "VRPlayerController.h"
#include "../CaptureTheChest/GMCaptureTheChest.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/Pawn.h"
#include "Runtime/Engine/Classes/Engine/World.h"


AVRPlayerController::AVRPlayerController()
{
	bReplicates = true;
}


//void AVRPlayerController::SignalToGameMode()
//{
//	SERVER_SignalToGameMode();
//}
//
//void AVRPlayerController::SERVER_SignalToGameMode_Implementation()
//{
//	AGMCaptureTheChest* pGameMode = dynamic_cast<AGMCaptureTheChest*>(GetWorld()->GetAuthGameMode());
//	if (pGameMode)
//	{
//		pGameMode->AttachPlayerControllerToShip(this);
//	}
//}
//
//bool AVRPlayerController::SERVER_SignalToGameMode_Validate()
//{
//	return true;
//}

void AVRPlayerController::BeginPlayingState()
{
	// Is this called on both the server and clients?
	Super::BeginPlayingState();
	
	// Only on server (called on both server and client)
	// Broadcast the player controller has a valid pawn
	if (HasAuthority() && OnInitialPossesPawn.IsBound())
	{
		OnInitialPossesPawn.Broadcast(this);
		// Remove all added delegates after being broadcasted once ("On--INITIAL--PossesPawn")
		OnInitialPossesPawn.RemoveAll(this);
	}

}

void AVRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("BecomeCinematicPawn", EInputEvent::IE_Pressed, this, &AVRPlayerController::SpawnAndPossessCinematicPawn);

}

void AVRPlayerController::SERVER_SpawnAndPossesCinematicPawn_Implementation()
{
	APawn* pPawn{ GetPawn() };
	if (pPawn)
	{
		FVector currentLocation{ pPawn->GetActorLocation() };
		FRotator currentRotation{ pPawn->GetActorRotation() };
		UnPossess();
		pPawn->Destroy();

		//CLIENT_PossessCinematicPawn();

		if (!m_CinematicPawnClass)
		{
			m_CinematicPawnClass = APawn::StaticClass();
		}

		FActorSpawnParameters actorSpawnParams{};
		actorSpawnParams.Owner = this;
		//actorSpawnParams.Owner

		APawn* pCinematicPawn{ GetWorld()->SpawnActor<APawn>(m_CinematicPawnClass,currentLocation, currentRotation,actorSpawnParams) };
		if (pCinematicPawn)
		{
			Possess(pCinematicPawn);
		}
	}
}

bool AVRPlayerController::SERVER_SpawnAndPossesCinematicPawn_Validate()
{
	return true;
}

void AVRPlayerController::SpawnAndPossessCinematicPawn()
{
	SERVER_SpawnAndPossesCinematicPawn();
}

void AVRPlayerController::CLIENT_PossessCinematicPawn_Implementation()
{
	if (!m_CinematicPawnClass)
	{
		m_CinematicPawnClass = APawn::StaticClass();
	}
	FActorSpawnParameters actorSpawnParams{};
	actorSpawnParams.Owner = this;
	//actorSpawnParams.Owner

	APawn* pCinematicPawn{ GetWorld()->SpawnActor<APawn>(actorSpawnParams) };
	if (pCinematicPawn)
	{
		Possess(pCinematicPawn);
	}
	

}
bool AVRPlayerController::CLIENT_PossessCinematicPawn_Validate()
{
	return true;
}