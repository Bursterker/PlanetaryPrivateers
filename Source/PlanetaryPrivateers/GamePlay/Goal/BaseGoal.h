// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseGoal.generated.h"

class USphereComponent;
UCLASS()
class PLANETARYPRIVATEERS_API ABaseGoal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseGoal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	/**
	*Called when overlapping with another actor
	* @Param							OverlappedActor							Self
	* @Param							OtherActor									The actor overlapping witht his actor
	*/
	UFUNCTION()
		virtual void OnOverlapBegin(class UPrimitiveComponent* pThisComponent, class AActor* pOtherActor, UPrimitiveComponent* pOtherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult);

	/**Plays the given effects on all clients*/
	UFUNCTION(Server, Reliable, WithValidation)
	void SERVER_PlayOnScoreEffects();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* m_pMesh = nullptr;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* m_pDeliveryHitBox {nullptr};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		class UParticleSystemComponent* m_pParticleSystem = nullptr;
};
