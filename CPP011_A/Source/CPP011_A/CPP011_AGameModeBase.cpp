// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPP011_AGameModeBase.h"
#include "MyDefaultPawn.h"

ACPP011_AGameModeBase::ACPP011_AGameModeBase() {
	DefaultPawnClass = AMyDefaultPawn::StaticClass();
}