// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorWithWorldWidget_Dynamic.generated.h"

class UUserWidget;

UCLASS()
class CPP008_API AActorWithWorldWidget_Dynamic : public AActor {

	GENERATED_BODY()

public:

	AActorWithWorldWidget_Dynamic();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

protected:

	UPROPERTY(Category = "Actor with World Widget", EditAnywhere)
		FName LayerName;

	UPROPERTY(Category = "Actor with World Widget", EditAnywhere)
		int32 LayerZOrder;

	UPROPERTY(Category = "Actor with World Widget", EditAnywhere)
		TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(Category = "Actor with World Widget", EditAnywhere)
		FIntPoint DrawSize;

	UPROPERTY(Category = "Actor with World Widget", EditAnywhere)
		FVector2D Pivot;

	UPROPERTY(Category = "Actor with World Widget", EditAnywhere)
		uint8 bDrawAtDesiredSize : 1;

	FVector startLocation;
};
