// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPP010GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SGameLayerManager.h"
#include "Slate/SceneViewport.h"
#include "SMaskedImage.h"

void ACPP010GameModeBase::BeginPlay() {
	Super::BeginPlay();

	if (auto controller = UGameplayStatics::GetPlayerController(this, 0)) {
		auto localPlayer = Cast<ULocalPlayer>(controller->Player);
		if (localPlayer && localPlayer->ViewportClient) {
			auto layerManager = localPlayer->ViewportClient->GetGameLayerManager();
			if (layerManager.Get()) {
				layerManager->AddWidgetForPlayer(localPlayer, SAssignNew(MaskedImage, SMaskedImage).Image(&ImageBrush).MaskImage(&MaskImageBrush), 0);
			}
		}
	}
}

void ACPP010GameModeBase::EndPlay(EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	if (MaskedImage.IsValid()) {
		if (auto controller = UGameplayStatics::GetPlayerController(this, 0)) {
			auto localPlayer = Cast<ULocalPlayer>(controller->Player);
			if (localPlayer && localPlayer->ViewportClient) {
				auto layerManager = localPlayer->ViewportClient->GetGameLayerManager();
				if (layerManager.Get()) {
					layerManager->RemoveWidgetForPlayer(localPlayer, MaskedImage.ToSharedRef());
				}
			}
		}
		MaskedImage.Reset();
	}
}