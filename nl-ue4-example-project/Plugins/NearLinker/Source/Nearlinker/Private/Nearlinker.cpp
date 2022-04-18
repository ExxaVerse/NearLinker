// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#include"Nearlinker.h"
#include"Settings.h"
#include"FunctionLibrary.h"
#include"ISettingsModule.h"
#include"ISettingsSection.h"
#include"log.h"

#define LOCTEXT_NAMESPACE "FNearlinkerModule"

void FNearlinkerModule::StartupModule(){
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	this->RegisterSettings();
	if(GetMutableDefault<UNearlinkerSettings>()->auto_launch_local_server){
		UNearlinkerFunctionLibrary::StartIntegrationServer();
	}
}

void FNearlinkerModule::ShutdownModule(){
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UNearlinkerFunctionLibrary::StopIntegrationServer();
	this->UnregisterSettings();
}

void FNearlinkerModule::RegisterSettings(){
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")){
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "Nearlinker",
			LOCTEXT("RuntimeSettingsName", "Nearlinker"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Nearlinker plugin parameters"),
			GetMutableDefault<UNearlinkerSettings>()
		);
		if(!SettingsSection.IsValid()) return;

		SettingsSection->OnModified().BindRaw(this, &FNearlinkerModule::OnSettingsModified);
	}
}

bool FNearlinkerModule::OnSettingsModified(){
	return true;
}

void FNearlinkerModule::UnregisterSettings(){
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")){
		SettingsModule->UnregisterSettings("Project", "Plugins", "Nearlinker");
	}
}

#undef LOCTEXT_NAMESPACE
	
DEFINE_LOG_CATEGORY(LogNearlinker);

IMPLEMENT_MODULE(FNearlinkerModule, Nearlinker)
