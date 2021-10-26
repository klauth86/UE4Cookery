// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

UCLASS()
class CPP012_API AMyPlayerController : public APlayerController {

	GENERATED_UCLASS_BODY()

protected:

	virtual TSharedPtr<class SVirtualJoystick> CreateVirtualJoystick();
};