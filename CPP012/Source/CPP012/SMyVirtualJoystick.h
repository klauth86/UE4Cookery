// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/Input/SVirtualJoystick.h"

class CPP012_API SMyVirtualJoystick : public SVirtualJoystick {
	
public:

	SLATE_BEGIN_ARGS(SMyVirtualJoystick) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};