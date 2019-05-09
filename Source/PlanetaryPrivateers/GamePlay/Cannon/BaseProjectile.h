// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class AVRPawn;

UCLASS()
class PLANETARYPRIVATEERS_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

	// should be set protected if MC_AddImpulse works
	UFUNCTION(BlueprintCallable)
	void AddImpulse(float force, FVector direction);

	UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable)
	void MC_AddImpulse(float force, FVector direction);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadWrite, Replicated)
	AActor* ObjectThatFiredMe = nullptr;
	UPROPERTY(BlueprintReadWrite, Replicated)
	AVRPawn* PlayerThatFiredMe = nullptr;


private:
	//FTransform m_SeverTransform{};
	//
	//UFUNCTION()
	//void OnRep_ServerTransform();

public:
	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
};
