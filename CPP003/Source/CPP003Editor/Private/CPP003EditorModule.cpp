#include "CPP003EditorModule.h"
#include "MyActorComponent.h"
#include "ComponentVisualizers/ComponentVisualizer_MyActorComp.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"

DEFINE_LOG_CATEGORY(LogCPP003Editor);

#define LOCTEXT_NAMESPACE "FCPP003EditorModule"

void FCPP003EditorModule::StartupModule() {
	RegisterComponentVisualizer(UMyActorComponent::StaticClass()->GetFName(), MakeShareable(new FComponentVisualizer_MyActorComp));
}

void FCPP003EditorModule::ShutdownModule() {
	if (GUnrealEd != NULL) {
		// Iterate over all class names we registered for
		for (FName ClassName : RegisteredComponentClassNames) {
			GUnrealEd->UnregisterComponentVisualizer(ClassName);
		}
	}
}

void FCPP003EditorModule::RegisterComponentVisualizer(FName ComponentClassName, TSharedPtr<FComponentVisualizer> Visualizer) {
	if (GUnrealEd != NULL) {
		GUnrealEd->RegisterComponentVisualizer(ComponentClassName, Visualizer);
	}

	RegisteredComponentClassNames.Add(ComponentClassName);

	if (Visualizer.IsValid()) {
		Visualizer->OnRegister();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCPP003EditorModule, CPP003Editor);