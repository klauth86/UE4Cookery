#pragma once

#include "UObject/ObjectMacros.h"
#include "UObject/NameTypes.h"
#include "MyDictionary.generated.h"

USTRUCT(BlueprintType)
struct CPP005_API FMyDictionary {

	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, Category = "My Dictionary")
	uint8 ItemIndex;
	
	static TArray<FName> Items;
};