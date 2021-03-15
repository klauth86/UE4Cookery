// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MyObject_Base.h"
#include "MyObject_B.generated.h"

UCLASS()
class CPP006_API UMyObject_B : public UMyObject_Base
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere)
	uint8 MyIntProp = 0;
};
