// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CPP009GameModeBase.generated.h"

class STextBlock;

UCLASS()
class CPP009_API ACPP009GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type reason) override;

protected:

	TSharedPtr<STextBlock> MyTextBlock;
};
