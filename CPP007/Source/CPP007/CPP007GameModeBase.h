// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "CPP007GameModeBase.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class CPP007_API ACPP007GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay();

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> AddToViewportWidgetClass;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> AddToPlayerScreenWidgetClass;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> LayerManagerWidgetClass;

	UPROPERTY()
		UUserWidget* LayerManagerWidget;
};
