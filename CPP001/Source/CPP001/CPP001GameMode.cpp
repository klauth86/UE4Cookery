// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CPP001GameMode.h"
#include "CPP001Character.h"
#include "UObject/ConstructorHelpers.h"

ACPP001GameMode::ACPP001GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
