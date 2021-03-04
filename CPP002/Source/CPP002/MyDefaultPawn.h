// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/DefaultPawn.h"
#include "MyDefaultPawn.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FToggleMenuEvent, AMyDefaultPawn*)

UCLASS()
class CPP002_API AMyDefaultPawn : public ADefaultPawn {

	GENERATED_BODY()

public:

	static FToggleMenuEvent OnToggleMenu;

protected:

	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

	void ToggleMenu() { OnToggleMenu.Broadcast(this); }
};
