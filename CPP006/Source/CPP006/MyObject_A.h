// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MyObject_Base.h"
#include "MyObject_A.generated.h"

UCLASS()
class CPP006_API UMyObject_A : public UMyObject_Base
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
		FString MyString = "Hello A!";
};
