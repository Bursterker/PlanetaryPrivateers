// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TreasureChest.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class ABaseProjectile;

UCLASS()
class PLANETARYPRIVATEERS_API ATreasureChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreasureChest();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	/**
 *Called when overlapping with another actor
 * @Param							OverlappedActor							Self
 * @Param							OtherActor									The actor overlapping witht his actor
 */
	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* pThisComponent, class AActor* pOtherActor, UPrimitiveComponent* pOtherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult);
	
	/**
	 * Places this chest on the ship corresponding to who shot the projectile
	 * @Param						pHookProjectile							The projectile this actor has to attach to
	 */
	UFUNCTION(BlueprintCallable)
	void PlaceChestOnShip(ABaseProjectile* pHookProjectile);

	/**
	 *Switches the particle of the ParticleSystemComponent
	 * @Param						pParticleSystem							The particle to be changed to
	 */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void SERVER_SwitchParticle(UParticleSystem* pParticleSystem);

public:
	UPROPERTY(BlueprintReadWrite)
	int m_OwningTeamId = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
		UParticleSystem* m_pParticleInWorld = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
		UParticleSystem* m_pParticleOnShip = nullptr;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		class UParticleSystemComponent* m_pParticleSystem = nullptr;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* m_pHitBox;

private:

};
