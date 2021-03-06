// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"
#include "Engine/World.h"

// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyActor::Test1() {
	if (Test1Actor) {
		int i = 10;
		while (i-- > 0) {
			auto offset = 400 * (FVector2D::UnitVector + FMath::RandPointInCircle(1));
			auto location = FVector(offset, 0) + GetActorLocation();
			GetWorld()->SpawnActor(Test1Actor->GetClass(), &location, &FRotator::ZeroRotator);
		}
	}
}