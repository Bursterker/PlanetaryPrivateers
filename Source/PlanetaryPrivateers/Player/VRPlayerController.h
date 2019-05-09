// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VRPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInitialPossesPawn, APlayerController*, pController);

/**
 * 
 */
UCLASS()
class PLANETARYPRIVATEERS_API AVRPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AVRPlayerController();

	FOnInitialPossesPawn OnInitialPossesPawn;

	//UFUNCTION(BlueprintCallable)
	//void SignalToGameMode();
	//
	//UFUNCTION(Server, Reliable, WithValidation)
	//void SERVER_SignalToGameMode();



protected:
	virtual void BeginPlayingState() override;

	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> m_CinematicPawnClass{ nullptr };

	void SpawnAndPossessCinematicPawn();
	UFUNCTION(Server, Reliable, WithValidation)
	void SERVER_SpawnAndPossesCinematicPawn();

	UFUNCTION(Client, Reliable, WithValidation)
	void CLIENT_PossessCinematicPawn();

};
