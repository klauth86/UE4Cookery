#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCPP003Editor, Log, All);

class FComponentVisualizer;

class FCPP003EditorModule : public IModuleInterface {
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:

	void RegisterComponentVisualizer(FName ComponentClassName, TSharedPtr<FComponentVisualizer> Visualizer);

protected:

	TArray<FName> RegisteredComponentClassNames;
};