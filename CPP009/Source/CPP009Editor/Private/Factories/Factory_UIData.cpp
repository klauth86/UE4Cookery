// Fill out your copyright notice in the Description page of Project Settings.

#include "Factories/Factory_UIData.h"
#include "UIData.h"
#include "AssetTypeCategories.h"

#define LOCTEXT_NAMESPACE "Factory_UIData"

UFactory_UIData::UFactory_UIData(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UUIData::StaticClass();
}

FText UFactory_UIData::GetDisplayName() const {
	return FText::Format(LOCTEXT("Factory_UIDataDisplayName", "UI Data"), NULL);
}

uint32 UFactory_UIData::GetMenuCategories() const { return EAssetTypeCategories::Blueprint; }

UObject* UFactory_UIData::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) {
	auto NewObjectAsset = NewObject<UUIData>(InParent, InClass, InName, Flags);
	return NewObjectAsset;
}

#undef LOCTEXT_NAMESPACE