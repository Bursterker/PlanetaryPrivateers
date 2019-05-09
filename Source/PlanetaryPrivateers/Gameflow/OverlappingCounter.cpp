// Fill out your copyright notice in the Description page of Project Settings.

#include "OverlappingCounter.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

// ===================================================
AOverlappingCounter::AOverlappingCounter()
{
	//RegisterEvents
	OnActorBeginOverlap.AddDynamic(this, &AOverlappingCounter::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &AOverlappingCounter::OnOverlapEnd);
}

// ===================================================
void AOverlappingCounter::BeginPlay()
{
	Super::BeginPlay();

}

// ===================================================
void AOverlappingCounter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DrawDebugLines();
}

// ===================================================
void AOverlappingCounter::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	/**Check if the Actors do not equal to nullptr*/
	if (OtherActor && (OtherActor != this))
	{
		/**Add th actor to the overlappingArray*/
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("OverlappingCounter.cpp::OnOverlapBegin --> Started overlapping with: %s"), *OtherActor->GetName()));
		m_tpOverlappingActors.Add(OtherActor);
	}
}

// ===================================================
void AOverlappingCounter::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	/**Check if the actors do not equal to nullptr*/
	if (OtherActor && (OtherActor != this))
	{
		/**Remove the actor from the overlappingArray*/
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("OverlappingCounter.cpp::OnOverlapEnd --> Stopped overlapping with: %s"), *OtherActor->GetName()));
		m_tpOverlappingActors.Remove(OtherActor);
	}
}

// ===================================================
void AOverlappingCounter::DrawDebugLines()
{
	/**Make the trigger visible in PIE*/
	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Purple, true, -1, 0, 5);
}
