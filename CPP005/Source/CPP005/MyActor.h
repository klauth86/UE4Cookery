// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MyDictionary.h"
#include "MyActor.generated.h"

UCLASS()
class CPP005_API AMyActor : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "My Actor")
		FMyDictionary MyDict;
};
