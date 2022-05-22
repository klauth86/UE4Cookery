// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/DefaultPawn.h"
#include "MyDefaultPawn.generated.h"

UCLASS()
class CPP011_A_API AMyDefaultPawn : public ADefaultPawn
{
	GENERATED_BODY()	

protected:

	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

	void OnPause();
};