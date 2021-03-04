// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenuWidget.h"
#include "MyDefaultPawn.h"

void UGameMenuWidget::NativeConstruct() {
	Super::NativeConstruct();
	AMyDefaultPawn::OnToggleMenu.AddUObject(this, &UGameMenuWidget::OnToggleMenu);
}

void UGameMenuWidget::NativeDestruct() {
	AMyDefaultPawn::OnToggleMenu.RemoveAll(this);
	Super::NativeDestruct();
}