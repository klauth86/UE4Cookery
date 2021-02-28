// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPathFollowingComponent.h"

void UMyPathFollowingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UE_LOG(LogTemp, Warning, TEXT("I am MyPathFollowingComponent!"));
}