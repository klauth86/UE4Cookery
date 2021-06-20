#include "CPP009EditorModule.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions/AssetTypeActions_UIData.h"

DEFINE_LOG_CATEGORY(LogCPP009Editor);

#define LOCTEXT_NAMESPACE "FCPP009EditorModule"

void FCPP009EditorModule::StartupModule() {
	RegisteredAssetTypeActions.Add(MakeShared<FAssetTypeActions_UIData>());

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	for (auto& registeredAssetTypeAction : RegisteredAssetTypeActions) {
		if (registeredAssetTypeAction.IsValid()) {
			AssetTools.RegisterAssetTypeActions(registeredAssetTypeAction.ToSharedRef());
		}
	}
}

void FCPP009EditorModule::ShutdownModule() {
	if (FModuleManager::Get().IsModuleLoaded("AssetTools")) {
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (auto& registeredAssetTypeAction : RegisteredAssetTypeActions) {
			if (registeredAssetTypeAction.IsValid()) {
				AssetTools.UnregisterAssetTypeActions(registeredAssetTypeAction.ToSharedRef());
				registeredAssetTypeAction.Reset();
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCPP009EditorModule, CPP009Editor);