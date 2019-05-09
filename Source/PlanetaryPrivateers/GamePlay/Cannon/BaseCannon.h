// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseCannon.generated.h"

UCLASS()
class PLANETARYPRIVATEERS_API ABaseCannon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseCannon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**Play Animation/Particles/... when fired*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MULTICAST_PlayFireEffects();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystemComponent* m_FireParticleEmitter = nullptr;
};
