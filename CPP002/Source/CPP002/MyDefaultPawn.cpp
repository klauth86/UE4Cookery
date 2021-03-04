// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDefaultPawn.h"
#include "Components/InputComponent.h"

FToggleMenuEvent AMyDefaultPawn::OnToggleMenu;

void AMyDefaultPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent) {
	Super::SetupPlayerInputComponent(InInputComponent);

	InInputComponent->BindAction("ToggleMenu", IE_Pressed, this, &AMyDefaultPawn::ToggleMenu);
}