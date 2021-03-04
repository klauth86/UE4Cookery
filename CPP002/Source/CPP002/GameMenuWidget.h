// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GameMenuWidget.generated.h"

class AMyDefaultPawn;

UCLASS()
class CPP002_API UGameMenuWidget : public UUserWidget {

	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, category = "Game Menu Widget")
	void OnToggleMenu(AMyDefaultPawn* pawn);

protected:

	virtual void NativeConstruct();

	virtual void NativeDestruct();
};
