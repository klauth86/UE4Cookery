// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorWithWorldWidget_Dynamic.h"
#include "WorldWidgetsManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"

// Sets default values
AActorWithWorldWidget_Dynamic::AActorWithWorldWidget_Dynamic() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LayerName = FName(StaticClass()->GetName());
	LayerZOrder = -100;
	DrawSize = FIntPoint(512, 512);
	Pivot = FVector2D(0.5f, 0.5f);
	bDrawAtDesiredSize = true;
}

void AActorWithWorldWidget_Dynamic::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	const auto timeSec = GetWorld()->TimeSeconds;
	SetActorLocation(startLocation + 100 * FVector(0, FMath::Sin(timeSec), FMath::Cos(timeSec)));
}

// Called when the game starts or when spawned
void AActorWithWorldWidget_Dynamic::BeginPlay() {
	Super::BeginPlay();

	startLocation = GetActorLocation();

	auto world = GetWorld();

	if (WidgetClass) {
		if (auto widget = CreateWidget<UUserWidget>(world, WidgetClass)) {
			WorldWidgetsManager::AddWorldWidget(world, 0, LayerName, LayerZOrder, this, widget->TakeWidget(), DrawSize, Pivot, bDrawAtDesiredSize, FVector::ZeroVector, this);
		}
	}
}

void AActorWithWorldWidget_Dynamic::EndPlay(EEndPlayReason::Type EndPlayReason) {

	auto world = GetWorld();

	WorldWidgetsManager::RemoveWorldWidget(world, 0, LayerName, this);

	Super::EndPlay(EndPlayReason);
}