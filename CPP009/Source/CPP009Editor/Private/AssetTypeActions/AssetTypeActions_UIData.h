// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AssetTypeActions_Base.h"

class FAssetTypeActions_UIData : public FAssetTypeActions_Base {
public:
	//~ Begin IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UIData", "Game Menu Data"); }
	virtual FColor GetTypeColor() const override { return FColor(97, 97, 85); }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Blueprint; }
	virtual UClass* GetSupportedClass() const override;
	virtual bool CanFilter() override { return true; }
	virtual bool IsImportedAsset() const override { return false; }
	virtual TSharedPtr<class SWidget> GetThumbnailOverlay(const FAssetData& AssetData) const override { return nullptr; }
	//~ End IAssetTypeActions Implementation
};