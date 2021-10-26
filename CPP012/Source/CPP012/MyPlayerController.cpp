// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "SMyVirtualJoystick.h"

AMyPlayerController::AMyPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

TSharedPtr<SVirtualJoystick> AMyPlayerController::CreateVirtualJoystick() {
	return SNew(SMyVirtualJoystick);
}