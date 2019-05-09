// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoContainerBase.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UInteractableComponent;
class ABaseProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoLeftChanged, int32, nbAmmoLeft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEmpty);

UCLASS()
class PLANETARYPRIVATEERS_API AAmmoContainerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoContainerBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* m_pGrabTrigger{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* m_pContainerMesh{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInteractableComponent* m_pGrabInteractable{nullptr};


public:
	UFUNCTION(BlueprintCallable)
	int32 GetAmmoLeft() { return m_CurrentAmmoLeft; }

	UFUNCTION(BlueprintCallable)
	void ConsumeAmmo();

	UPROPERTY(BlueprintAssignable)
	FOnEmpty OnEmpty;	
	UPROPERTY(BlueprintAssignable)
	FOnAmmoLeftChanged OnAmmoLeftChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo Settings")
	int32 m_MaxAmmoCapacity{ 5 };
private:
	UPROPERTY(Replicated)
	int32 m_CurrentAmmoLeft{ 0 };
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<ABaseProjectile> m_pProjectileClass{nullptr};
public:
	UFUNCTION(BlueprintCallable)
	TSubclassOf<ABaseProjectile> GetProjectileType() { return m_pProjectileClass; }

	// --- Loading ---
public:

	UFUNCTION(BlueprintCallable)
	bool GetIsLoaded() { return m_bLoaded; }
	UFUNCTION(BlueprintCallable)
	void LoadIntoCannon(AActor* pCannon);
	UFUNCTION(BlueprintCallable)
	void UnloadFromCannon(bool bEject = false, FVector ejectImpulse = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable)
	void EnablePhysics(bool bEnable);

protected:
	// Function to broadcast to clients and server
	UFUNCTION(NetMulticast, WithValidation, Reliable)
	void MC_LoadIntoCannon(AActor* pCannon);
	// Function to broadcast to clients and server
	UFUNCTION(NetMulticast, WithValidation, Reliable)
	void MC_UnloadFromCannon(bool bEject = false, FVector ejectImpulse = FVector::ZeroVector);


	// Local functionality
	UFUNCTION(BlueprintNativeEvent)
	void LoadIntoCannonLocal(AActor* pCannon);
// TEMP PUBLIC
	// Local functionality
	void UnloadFromCannonLocal(bool bEject = false, FVector ejectImpulse = FVector::ZeroVector);
	UPROPERTY(BlueprintReadWrite)
	bool m_bLoaded;

protected:
	UPROPERTY(BlueprintReadOnly)
	AActor* m_pLoadedInCannon{ nullptr };



public:
	UFUNCTION(BlueprintCallable)
	void AddImpulse(float force, FVector direction);

public:
	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
};
