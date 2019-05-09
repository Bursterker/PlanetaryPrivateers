// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseShip.generated.h"

class UBoxComponent; 
class ATreasureChest;

UCLASS()
class PLANETARYPRIVATEERS_API ABaseShip : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PlaceActorOnShip(AActor* pToAttach);

	/**Function that returns the transform for cargo*/
	FTransform GetCargoSpace();

	/**
	 *Return the child that is cargo
	 *@Return							returns first actor child, else nullptr if no childs
	 */
#pragma region GetCargoTemplate
	template<class T>
	T* GetCargo()
	{
		// Attaching to m_pCargoSpace messes up movement replication
		// AttachToActor works fine > need different way of keeping track of cargo
		
		for (AActor* pCargoItem : m_tpActorsInCargo)
		{
			T* pCorrectItem{ Cast<T>(pCargoItem) };
			if (pCorrectItem)
			{
				return pCorrectItem;
			}
		}
		
		return nullptr;
		//
		//
		////Check if there is a child actor
		//TArray<AActor*> childActors;
		//GetAllChildActors(childActors, false);
		//
		//if (childActors.Num() > 0)
		//{
		//	//Get the first actor of class T
		//	for(AActor* child : childActors)
		//	{
		//		//Check if the actor is of class T
		//		T* actor = dynamic_cast<T*>(child);
		//		if (actor) return actor;
		//	}
		//}
		//return nullptr;
	}
#pragma endregion

	// Return all cargo of type ATreasureChest
	// Remove this cargo from m_ptActorsInCargo
	UFUNCTION(BlueprintCallable)
	TArray<ATreasureChest*> ReleaseAllTreasureCargo();

	TArray<USceneComponent*> GetPlayerSpawnLocations() { return m_tpSpawnLocations; };

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* m_pCargoSpace = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* m_pShipBody; // Any primitive component to apply force to

	UFUNCTION(BlueprintCallable)
	void AddActorToCargo(AActor* pActor);
	
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> m_tpActorsInCargo;

public:
	int m_OwningTeamId = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* m_pSpawnLoc0 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* m_pSpawnLoc1 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* m_pSpawnLoc2 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* m_pSpawnLoc3 = nullptr;
private:
	TArray<USceneComponent*> m_tpSpawnLocations = {};
};

