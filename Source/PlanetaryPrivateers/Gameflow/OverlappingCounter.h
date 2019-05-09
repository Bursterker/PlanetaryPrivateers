// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "OverlappingCounter.generated.h"

/**
 * 
 */
UCLASS()
class PLANETARYPRIVATEERS_API AOverlappingCounter : public ATriggerBox
{
	GENERATED_BODY()

public:
		AOverlappingCounter();

protected:
	/**Called when the game starts, or when the actor is spawned*/
	virtual void BeginPlay() override;

	/**
	 *Called every Tick
	 * @Param							DeltaSeconds								Time between this and last frame
	 */
	virtual void Tick(float DeltaSeconds) override;

	/**
	 *Called when overlapping with another actor
	 * @Param							OverlappedActor							Self
	 * @Param							OtherActor									The actor overlapping witht his actor
	 */
	UFUNCTION()
		virtual void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	/**
	*Called when an overlapped actor stops overlapping
	* @Param							OverlappedActor							Self
	* @Param							OtherActor									The actor overlapping witht his actor
	*/
	UFUNCTION()
		virtual void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);

public:
	/**Returns the ammount of actors overlapping this trigger*/
	int32 GetAmmountOfOverlappingActors() { return m_tpOverlappingActors.Num(); }

	/**Returns an array with all overlapping actors*/
	TArray<AActor*> GetAllOverlappingActors() { return m_tpOverlappingActors; }

	template< class T>
#pragma region GetAllOverlappingOfClass
	/**
	 *Return an array of all overlapping actors of a specified class
	 *@Template								Func<ClassType>()							ClassType as class to return
	 */
	TArray<T*> GetAllOverlappingOfClass()
	{
		TArray<T*> foundActors = {};

		/**Iterate over each actor*/
		for (AActor* pActor : m_tpOverlappingActors)
		{
			/**Cast the actor to T*/
			T* pTemplate = dynamic_cast<T*>(pActor);

			/**If the cast succeeded add it to foundActorsArray, else do nothing*/
			if (pTemplate)
			{
				foundActors.Add(pTemplate);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString::Printf(TEXT("OverlappingCounter.cpp::GetAllOverlappingOfClass --> Actor was not of class T")));

				FString displaynameOfActor = pActor->GetName();
				GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("OverlappingCounter.cpp::GetAllOverlappingOfClass --> %s"), ToCStr(displaynameOfActor)));
			}
		}
		return foundActors;
	}
#pragma endregion

	/**Draw the outer edges of this actor for debugging use only*/
	void DrawDebugLines();

private:
	TArray<AActor*> m_tpOverlappingActors;
};
