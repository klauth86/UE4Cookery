// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPP009GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SGameLayerManager.h"
#include "Slate/SceneViewport.h"
#include "Widgets/Text/STextBlock.h"
#include "GameDataSingleton.h"
#include "UIData.h"

void ACPP009GameModeBase::BeginPlay() {
	Super::BeginPlay();

	auto uiData = UGameDataSingleton::GetInstance()->GetUIData();

	if (auto controller = UGameplayStatics::GetPlayerController(this, 0)) {
		auto localPlayer = Cast<ULocalPlayer>(controller->Player);
		if (localPlayer && localPlayer->ViewportClient) {
			auto layerManager = localPlayer->ViewportClient->GetGameLayerManager();
			if (layerManager.Get()) {
				layerManager->AddWidgetForPlayer(localPlayer, SAssignNew(MyTextBlock, STextBlock).Text(FText::FromString("Hello, Game Data Singleton!")).Font(uiData->GetMenuFontInfo()), 0);
			}
		}
	}
}

void ACPP009GameModeBase::EndPlay(EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	if (MyTextBlock.IsValid()) {	
		if (auto controller = UGameplayStatics::GetPlayerController(this, 0)) {
			auto localPlayer = Cast<ULocalPlayer>(controller->Player);
			if (localPlayer && localPlayer->ViewportClient) {
				auto layerManager = localPlayer->ViewportClient->GetGameLayerManager();
				if (layerManager.Get()) {
					layerManager->RemoveWidgetForPlayer(localPlayer, MyTextBlock.ToSharedRef());
				}
			}
		}
		MyTextBlock.Reset();
	}
}