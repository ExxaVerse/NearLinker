// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#pragma once

#include"Kismet/BlueprintFunctionLibrary.h"
#include"Templates/UniquePtr.h"
#include"Interfaces/IHttpRequest.h"

#include"FunctionLibrary.generated.h"

class FMonitoredProcess;

USTRUCT(BlueprintType)
struct FFunctionCallDescription{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString,FString> parameters;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FNearHttpRequestCompleteDelegate, FString, Response, bool, bConnectedSuccessfully);

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
	UFUNCTION(BlueprintCallable, Category="Nearlinker", meta=(AutoCreateRefTerm="method,resource,data,wallet_authorization"))
	static void SendRequestToIntegrationServer(FString const& method, FString const& resource, FNearHttpRequestCompleteDelegate const& response_handler, FString const& wallet_authorization="", FString const& data="");

	//UFUNCTION(BlueprintCallable, Category="Nearlinker")
	//static void CreateWallet(FString const& wallet_name, FNearHttpRequestCompleteDelegate const& response_handler);
	//UFUNCTION(BlueprintCallable, Category="Nearlinker")
	//static void DeployContract(FNearContract const& contract, FString const& wallet_authorization, FNearHttpRequestCompleteDelegate const& response_handler);
	UFUNCTION(BlueprintCallable, Category="Nearlinker", meta=(AutoCreateRefTerm="contract_id"))
	static void ContractView(FString const& contract_id, FFunctionCallDescription const& function_description, FNearHttpRequestCompleteDelegate const& response_handler);
	UFUNCTION(BlueprintCallable, Category="Nearlinker", meta=(AutoCreateRefTerm="contract_id"))
	static void ContractCall(FString const& contract_id, FFunctionCallDescription const& function_description, FString const& wallet_authorization, FNearHttpRequestCompleteDelegate const& response_handler);
};
