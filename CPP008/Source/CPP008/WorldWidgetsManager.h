#pragma once

#include "CoreTypes.h"
#include "Templates/SharedPointer.h"
#include "UObject/WeakObjectPtrTemplates.h"

class UWorld;
class UObject;
class SWidget;
class AActor;

class WorldWidgetsManager {

public:

	static void AddWorldWidget(UWorld* world, int32 playerControllerIndex, const FName& layerName, const int32 layerZOrder, UObject* owningObject, TSharedPtr<SWidget> widget, const FIntPoint& drawSize, const FVector2D& pivot, bool bDrawAtDesiredSize, const FVector& worldLocation, AActor* actor = nullptr);

	static void RemoveWorldWidget(UWorld* world, int32 playerControllerIndex, FName& layerName, UObject* owningObject);

	static bool HasWorldWidget(UWorld* world, int32 playerControllerIndex, FName& layerName, UObject* owningObject);

	static void Reset(UWorld* world, int32 playerControllerIndex);
};