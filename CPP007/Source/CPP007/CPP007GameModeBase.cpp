// Copyright Epic Games, Inc. All Rights Reserved.


#include "CPP007GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SGameLayerManager.h"

void ACPP007GameModeBase::BeginPlay() {
	Super::BeginPlay();

	auto world = GetWorld();

	if (AddToViewportWidgetClass_n10) {
		if (auto AddToViewportWidget = CreateWidget<UUserWidget>(world, AddToViewportWidgetClass_n10)) {
			AddToViewportWidget->AddToViewport(-10);
		}
	}

	if (AddToViewportWidgetClass_0) {
		if (auto AddToViewportWidget = CreateWidget<UUserWidget>(world, AddToViewportWidgetClass_0)) {
			AddToViewportWidget->AddToViewport();
		}
	}

	if (AddToViewportWidgetClass_p10) {
		if (auto AddToViewportWidget = CreateWidget<UUserWidget>(world, AddToViewportWidgetClass_p10)) {
			AddToViewportWidget->AddToViewport(10);
		}
	}

	auto controller = UGameplayStatics::GetPlayerController(this, 0);
	if (AddToPlayerScreenWidgetClass_n10 && controller) {
		if (auto AddToPlayerScreenWidget = CreateWidget<UUserWidget>(controller, AddToPlayerScreenWidgetClass_n10)) {
			AddToPlayerScreenWidget->AddToPlayerScreen(-10);
		}
	}

	if (AddToPlayerScreenWidgetClass_0 && controller) {
		if (auto AddToPlayerScreenWidget = CreateWidget<UUserWidget>(controller, AddToPlayerScreenWidgetClass_0)) {
			AddToPlayerScreenWidget->AddToPlayerScreen();
		}
	}

	if (AddToPlayerScreenWidgetClass_p10 && controller) {
		if (auto AddToPlayerScreenWidget = CreateWidget<UUserWidget>(controller, AddToPlayerScreenWidgetClass_p10)) {
			AddToPlayerScreenWidget->AddToPlayerScreen(10);
		}
	}

	if (auto ViewportClient = world->GetGameViewport()) {

		if (ViewportWidgetClass_n10) {
			ViewportWidget_n10 = CreateWidget<UUserWidget>(world, ViewportWidgetClass_n10);
			if (ViewportWidget_n10) {
				ViewportClient->AddViewportWidgetContent(ViewportWidget_n10->TakeWidget(), -10);
			}
		}

		if (ViewportWidgetClass_0) {
			ViewportWidget_0 = CreateWidget<UUserWidget>(world, ViewportWidgetClass_0);
			if (ViewportWidget_0) {
				ViewportClient->AddViewportWidgetContent(ViewportWidget_0->TakeWidget());
			}
		}

		if (ViewportWidgetClass_p10) {
			ViewportWidget_p10 = CreateWidget<UUserWidget>(world, ViewportWidgetClass_p10);
			if (ViewportWidget_p10) {
				ViewportClient->AddViewportWidgetContent(ViewportWidget_p10->TakeWidget(), 10);
			}
		}

		if (auto localPlayer = controller->GetLocalPlayer()) {
			if (ViewportWidgetForPlayerClass_n10) {
				ViewportWidgetForPlayer_n10 = CreateWidget<UUserWidget>(world, ViewportWidgetForPlayerClass_n10);
				if (ViewportWidgetForPlayer_n10) {
					ViewportClient->AddViewportWidgetForPlayer(localPlayer, ViewportWidgetForPlayer_n10->TakeWidget(), -10);
				}
			}

			if (ViewportWidgetForPlayerClass_0) {
				ViewportWidgetForPlayer_0 = CreateWidget<UUserWidget>(world, ViewportWidgetForPlayerClass_0);
				if (ViewportWidgetForPlayer_0) {
					ViewportClient->AddViewportWidgetForPlayer(localPlayer, ViewportWidgetForPlayer_0->TakeWidget(), 0);
				}
			}

			if (ViewportWidgetForPlayerClass_p10) {
				ViewportWidgetForPlayer_p10 = CreateWidget<UUserWidget>(world, ViewportWidgetForPlayerClass_p10);
				if (ViewportWidgetForPlayer_p10) {
					ViewportClient->AddViewportWidgetForPlayer(localPlayer, ViewportWidgetForPlayer_p10->TakeWidget(), 10);
				}
			}

			if (auto manager = ViewportClient->GetGameLayerManager()) {

				if (LayerManagerWidgetClass_n10 && controller) {
					LayerManagerWidget_n10 = CreateWidget<UUserWidget>(controller, LayerManagerWidgetClass_n10);
					if (LayerManagerWidget_n10) {
						manager->AddWidgetForPlayer(localPlayer, LayerManagerWidget_n10->TakeWidget(), 0);
					}
				}

				if (LayerManagerWidgetClass_0 && controller) {
					LayerManagerWidget_0 = CreateWidget<UUserWidget>(controller, LayerManagerWidgetClass_0);
					if (LayerManagerWidget_0) {
						manager->AddWidgetForPlayer(localPlayer, LayerManagerWidget_0->TakeWidget(), 0);
					}
				}

				if (LayerManagerWidgetClass_p10 && controller) {
					LayerManagerWidget_p10 = CreateWidget<UUserWidget>(controller, LayerManagerWidgetClass_p10);
					if (LayerManagerWidget_p10) {
						manager->AddWidgetForPlayer(localPlayer, LayerManagerWidget_p10->TakeWidget(), 0);
					}
				}
			}
		}
	}
}