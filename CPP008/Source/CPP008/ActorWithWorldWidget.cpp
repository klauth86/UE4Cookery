// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorWithWorldWidget.h"
#include "WorldWidgetsManager.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AActorWithWorldWidget::AActorWithWorldWidget() {
	LayerName = FName(StaticClass()->GetName());
	LayerZOrder = -100;
	DrawSize = FIntPoint(512, 512);
	Pivot = FVector2D(0.5f, 0.5f);
	bDrawAtDesiredSize = true;
}

// Called when the game starts or when spawned
void AActorWithWorldWidget::BeginPlay()
{
	Super::BeginPlay();
	
	auto world = GetWorld();

	if (WidgetClass) {
		if (auto widget = CreateWidget<UUserWidget>(world, WidgetClass)) {
			WorldWidgetsManager::AddWorldWidget(world, 0, LayerName, LayerZOrder, this, widget->TakeWidget(), DrawSize, Pivot, bDrawAtDesiredSize, GetActorLocation());
		}
	}
}

void AActorWithWorldWidget::EndPlay(EEndPlayReason::Type EndPlayReason) {

	auto world = GetWorld();

	WorldWidgetsManager::RemoveWorldWidget(world, 0, LayerName, this);

	Super::EndPlay(EndPlayReason);
}