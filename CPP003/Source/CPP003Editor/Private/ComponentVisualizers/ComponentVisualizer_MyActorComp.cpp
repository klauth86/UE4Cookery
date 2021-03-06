// Fill out your copyright notice in the Description page of Project Settings.


#include "ComponentVisualizer_MyActorComp.h"
#include "EngineUtils.h"

void FComponentVisualizer_MyActorComp::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) {
	if (Component) {
		if (auto owner = Component->GetOwner()) {
			DrawWireSphere(PDI, owner->GetActorLocation(), FColor::Blue, 128, 16, SDPG_World, 2);
		}
	}
}