// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetTypeActions/AssetTypeActions_UIData.h"
#include "UIData.h"

UClass* FAssetTypeActions_UIData::GetSupportedClass() const { return UUIData::StaticClass(); }