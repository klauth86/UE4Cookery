// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "CPP010GameModeBase.generated.h"

class SMaskedImage;

UCLASS()
class CPP010_API ACPP010GameModeBase : public AGameModeBase {

	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type reason) override;

protected:

	UPROPERTY(EditDefaultsOnly)
		FSlateBrush ImageBrush;

	UPROPERTY(EditDefaultsOnly)
		FSlateBrush MaskImageBrush;

	TSharedPtr<SMaskedImage> MaskedImage;
};