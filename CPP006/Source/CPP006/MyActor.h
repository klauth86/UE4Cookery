// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

class UMyObject_Base;

UCLASS()
class CPP006_API AMyActor : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "My Actor", meta = (ShowOnlyInnerProperties), Instanced)
		UMyObject_Base* MyObj;

	UPROPERTY(EditAnywhere, Category = "My Actor", meta = (ShowOnlyInnerProperties), Instanced)
		TArray<UMyObject_Base*> MyObjArray;
};
