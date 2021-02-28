// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "MyPathFollowingComponent.h"

AMyAIController::AMyAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UMyPathFollowingComponent>(TEXT("PathFollowingComponent"))) {}
