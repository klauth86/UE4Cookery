// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Navigation/PathFollowingComponent.h"
#include "MyPathFollowingComponent.generated.h"

UCLASS()
class CPP001_API UMyPathFollowingComponent : public UPathFollowingComponent {

	GENERATED_BODY()
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
