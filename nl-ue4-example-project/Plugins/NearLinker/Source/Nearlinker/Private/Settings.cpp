// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#include"Settings.h"

UNearlinkerSettings::UNearlinkerSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->contracts_directories.Add(TEXT("Content/NEAR contracts"));
}
