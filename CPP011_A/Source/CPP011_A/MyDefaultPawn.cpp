// Fill out your copyright notice in the Description page of Project Settings.

#include "MyDefaultPawn.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"

const FName InputAction_Pause = FName("Pause");

void AMyDefaultPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping(InputAction_Pause, EKeys::P));

	InInputComponent->BindAction(InputAction_Pause, EInputEvent::IE_Pressed, this, &AMyDefaultPawn::OnPause);
}

void AMyDefaultPawn::OnPause()
{
	bool isNowPaused = !UGameplayStatics::IsGamePaused(this);

	UGameplayStatics::SetGamePaused(this, isNowPaused);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(isNowPaused ? "GAME IS PAUSED!" : "GAME IS OK!"));
}