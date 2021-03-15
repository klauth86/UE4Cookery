#include "CPP005EditorModule.h"
#include "PropertyEditorModule.h"
#include "MyDictionary.h"
#include "DetailCustomizations/PropertyTypeCustomization_MyDictionary.h"

DEFINE_LOG_CATEGORY(LogCPP005Editor);

#define LOCTEXT_NAMESPACE "FCPP005EditorModule"

void FCPP005EditorModule::StartupModule() {
	// Register the details customizer
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(FMyDictionary::StaticStruct()->GetFName(), 
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPropertyTypeCustomization_MyDictionary::MakeInstance));
}

void FCPP005EditorModule::ShutdownModule() {
	// Unregister the details customization
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor")) {
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FMyDictionary::StaticStruct()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCPP005EditorModule, CPP005Editor);