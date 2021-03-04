// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenuWidget.h"
#include "UIDelegates.h"

void UGameMenuWidget::NativeConstruct() {
	Super::NativeConstruct();
	UIDelegates::OnToggleMenu.AddUObject(this, &UGameMenuWidget::OnToggleMenu);
}

void UGameMenuWidget::NativeDestruct() {
	UIDelegates::OnToggleMenu.RemoveAll(this);
	Super::NativeDestruct();
}