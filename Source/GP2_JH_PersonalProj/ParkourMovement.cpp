// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourMovement.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
AParkourMovement::AParkourMovement()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


// Called when the game starts or when spawned
void AParkourMovement::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AParkourMovement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}


