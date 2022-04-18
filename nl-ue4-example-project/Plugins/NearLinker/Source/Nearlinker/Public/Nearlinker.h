// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FNearlinkerModule : public IModuleInterface
{
public:

	//IModuleInterface implementation
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	//Registration of Nearlinker section in project settings
	void RegisterSettings();
	void UnregisterSettings();
	bool OnSettingsModified();
};
