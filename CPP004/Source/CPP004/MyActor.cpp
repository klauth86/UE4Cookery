// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"
#include "Engine/World.h"

void AMyActor::Test1() {
	if (!Test1Actor) return;

	int i = 10;
	while (i-- > 0) {
		auto offset = 400 * (FVector2D::UnitVector + FMath::RandPointInCircle(1));
		auto location = FVector(offset, 0) + GetActorLocation();
		GetWorld()->SpawnActor(Test1Actor->GetClass(), &location, &FRotator::ZeroRotator);
	}
}