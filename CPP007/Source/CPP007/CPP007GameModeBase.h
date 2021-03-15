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
		TSubclassOf<UUserWidget> AddToViewportWidgetClass_n10;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> AddToViewportWidgetClass_0;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> AddToViewportWidgetClass_p10;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> AddToPlayerScreenWidgetClass_n10;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> AddToPlayerScreenWidgetClass_0;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> AddToPlayerScreenWidgetClass_p10;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> LayerManagerWidgetClass_n10;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> LayerManagerWidgetClass_0;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> LayerManagerWidgetClass_p10;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> ViewportWidgetClass_n10;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> ViewportWidgetClass_0;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> ViewportWidgetClass_p10;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> ViewportWidgetForPlayerClass_n10;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> ViewportWidgetForPlayerClass_0;

	UPROPERTY(EditDefaultsOnly, category = "CPP007 Game Mode")
		TSubclassOf<UUserWidget> ViewportWidgetForPlayerClass_p10;

	UPROPERTY()
		UUserWidget* LayerManagerWidget_n10;

	UPROPERTY()
		UUserWidget* LayerManagerWidget_0;

	UPROPERTY()
		UUserWidget* LayerManagerWidget_p10;

	UPROPERTY()
		UUserWidget* ViewportWidget_n10;

	UPROPERTY()
		UUserWidget* ViewportWidget_0;

	UPROPERTY()
		UUserWidget* ViewportWidget_p10;

	UPROPERTY()
		UUserWidget* ViewportWidgetForPlayer_n10;

	UPROPERTY()
		UUserWidget* ViewportWidgetForPlayer_0;

	UPROPERTY()
		UUserWidget* ViewportWidgetForPlayer_p10;
};
