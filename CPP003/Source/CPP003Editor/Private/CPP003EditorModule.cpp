#include "CPP003EditorModule.h"
#include "MyActorComponent.h"
#include "ComponentVisualizers/ComponentVisualizer_MyActorComp.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"

DEFINE_LOG_CATEGORY(LogCPP003Editor);

#define LOCTEXT_NAMESPACE "FCPP003EditorModule"

void FCPP003EditorModule::StartupModule() {
	GUnrealEd->RegisterComponentVisualizer(UMyActorComponent::StaticClass()->GetFName(), MakeShareable(new FComponentVisualizer_MyActorComp));
}

void FCPP003EditorModule::ShutdownModule() {
	GUnrealEd->UnregisterComponentVisualizer(UMyActorComponent::StaticClass()->GetFName());
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCPP003EditorModule, CPP003Editor);