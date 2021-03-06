#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCPP003Editor, Log, All);

class FCPP003EditorModule : public IModuleInterface {
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};