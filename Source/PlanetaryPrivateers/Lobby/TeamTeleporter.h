// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeamTeleporter.generated.h"

UCLASS()
class PLANETARYPRIVATEERS_API ATeamTeleporter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeamTeleporter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class AVRPawn* GetOverlappingPlayer();

private:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool CheckIfAPlayerOverlaps();

	void AddSelfToGameMode();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 m_TeamId = 0;

	bool m_HasPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* m_Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UParticleSystemComponent* m_pParticleSystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* m_TriggerBox = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 m_Id = -1;

private:
	TArray<AActor*> m_tpOverlappingActors;
};
