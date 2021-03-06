// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class CPP004_API AMyActor : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(Category = "MyActor: Test1", EditAnywhere)
	AActor* Test1Actor;

	UFUNCTION(Category = "MyActor: Test1", BlueprintCallable, CallInEditor)
	void Test1();
};
