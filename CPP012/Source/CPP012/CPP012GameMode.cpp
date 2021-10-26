// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPP012GameMode.h"
#include "CPP012HUD.h"
#include "CPP012Character.h"
#include "UObject/ConstructorHelpers.h"

ACPP012GameMode::ACPP012GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ACPP012HUD::StaticClass();
}
