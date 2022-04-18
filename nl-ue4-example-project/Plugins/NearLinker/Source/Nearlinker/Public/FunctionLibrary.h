// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#pragma once

#include"Kismet/BlueprintFunctionLibrary.h"
#include"Templates/UniquePtr.h"

#include"FunctionLibrary.generated.h"

class FMonitoredProcess;

UCLASS()
class NEARLINKER_API UNearlinkerFunctionLibrary : public UBlueprintFunctionLibrary{
	GENERATED_BODY()
 
//data
public:

//functions
public:
	UFUNCTION(BlueprintCallable, Category="Nearlinker")
	static void StartIntegrationServer();
	UFUNCTION(BlueprintCallable, Category="Nearlinker")
	static void StopIntegrationServer();
};
