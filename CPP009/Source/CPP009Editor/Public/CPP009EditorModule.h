#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCPP009Editor, Log, All);

class FCPP009EditorModule : public IModuleInterface {
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	TArray<TSharedPtr<class FAssetTypeActions_Base>> RegisteredAssetTypeActions;
};