// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#pragma once

#include"CoreMinimal.h"
#include"UObject/Object.h"

#include"Settings.generated.h"

USTRUCT(BlueprintType)
struct FProcessSettings{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString executable_path;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString args;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString working_directory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool start_hidden=true;
	//Process priority.
	//-2 idle, -1 low, 0 normal, 1 high, 2 higher
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//int32 priority=0;
};

UCLASS(config=Engine, defaultconfig)
class NEARLINKER_API UNearlinkerSettings : public UObject{
	GENERATED_UCLASS_BODY()
			
public:
	UPROPERTY(config, EditAnywhere, Category = "Integration Server")
	FProcessSettings server_launch_settings;
	UPROPERTY(config, EditAnywhere, Category = "Integration Server")
	bool auto_launch_local_server=true;
	UPROPERTY(config, EditAnywhere, Category = "Integration Server")
	FString server_url="https://localhost:3000";
	UPROPERTY(config, EditAnywhere, Category = "Contracts")
	TArray<FString> contracts_directories;
	//UPROPERTY(config, EditAnywhere, Category = "Contracts")
	//FProcessSettings compiler_settings;
};
