// Fill out your copyright notice in the Description page of Project Settings.

#include "Scoreboard.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AScoreboard::AScoreboard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	m_RenderPlaneT0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Team0 Plane"));
	m_RenderPlaneT0->SetupAttachment(RootComponent);

	m_RenderPlaneT1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Team1 Plane"));
	m_RenderPlaneT1->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AScoreboard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AScoreboard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

