// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "Fonts/SlateFontInfo.h"
#include "UIData.generated.h"

UCLASS()
class CPP009_API UUIData : public UObject {

	GENERATED_BODY()

public:

	const FSlateFontInfo& GetMenuFontInfo() const { return MenuFontInfo; }

	const FSlateFontInfo& GetGameFontInfo() const { return GameFontInfo; }

protected:

	UPROPERTY(EditDefaultsOnly, Category = "UI Data: Menu Font Info")
		FSlateFontInfo MenuFontInfo;

	UPROPERTY(EditDefaultsOnly, Category = "UI Data: Game Font Info")
		FSlateFontInfo GameFontInfo;
};