// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "GameDataSingleton.generated.h"

class UUIData;

UCLASS(Blueprintable)
class CPP009_API UGameDataSingleton : public UObject {

	GENERATED_UCLASS_BODY()

public:

	UUIData* GetUIData() const { return MyUIData; }

protected:

	UPROPERTY(EditDefaultsOnly)
		UUIData* MyUIData;

public:

	static UGameDataSingleton* GetInstance() { 
		static UGameDataSingleton* instance = Cast<UGameDataSingleton>(GEngine->GameSingleton);
		return instance;
	}
};