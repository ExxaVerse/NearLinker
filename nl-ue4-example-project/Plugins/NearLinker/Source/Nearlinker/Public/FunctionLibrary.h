// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#pragma once

#include"Kismet/BlueprintFunctionLibrary.h"
#include"Templates/UniquePtr.h"
#include"Interfaces/IHttpRequest.h"
#include<functional>

#include"FunctionLibrary.generated.h"

class FMonitoredProcess;

USTRUCT(BlueprintType)
struct FFunctionCallData{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString,FString> parameters;
};

USTRUCT(BlueprintType)
struct FContractCallData{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString account_id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString private_key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFunctionCallData function;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString gas;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString deposit;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FNearHttpRequestCompleteDelegate, FString, Response, bool, bConnectedSuccessfully);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FNearHttpGetIntCompleteDelegate, int, result, bool, bConnectedSuccessfully);

UCLASS()
class NEARLINKER_API UNearlinkerFunctionLibrary : public UBlueprintFunctionLibrary{
	GENERATED_BODY()
 
//data
public:

//functions
public:
	//Integration Server

	UFUNCTION(BlueprintCallable, Category="Nearlinker|Integration server")
	static void StartIntegrationServer();
	UFUNCTION(BlueprintCallable, Category="Nearlinker|Integration server")
	static void StopIntegrationServer();
	UFUNCTION(BlueprintCallable, Category="Nearlinker|Integration server", meta=(AutoCreateRefTerm="method,resource,data,wallet_authorization"))
	static void SendRequestToIntegrationServer(FString const& method, FString const& resource, FNearHttpRequestCompleteDelegate const& response_handler, FString const& data="");

	static void SendRequestToIntegrationServer(FString const& method, FString const& resource, std::function<void(FString,bool)> const& response_handler, FString const& data="");

	// Core blockchain funtions

	//UFUNCTION(BlueprintCallable, Category="Nearlinker")
	//static void CreateWallet(FString const& wallet_name, FNearHttpRequestCompleteDelegate const& response_handler);
	//UFUNCTION(BlueprintCallable, Category="Nearlinker")
	//static void DeployContract(FNearContract const& contract, FString const& wallet_authorization, FNearHttpRequestCompleteDelegate const& response_handler);
	UFUNCTION(BlueprintCallable, Category="Nearlinker", meta=(AutoCreateRefTerm="contract_id"))
	static void ContractView(FString const& contract_id, FFunctionCallData const& function_description, FNearHttpRequestCompleteDelegate const& response_handler);
	UFUNCTION(BlueprintCallable, Category="Nearlinker", meta=(AutoCreateRefTerm="contract_id"))
	static void ContractCall(FString const& contract_id, FFunctionCallData const& function_description, FString const& wallet_authorization, FNearHttpRequestCompleteDelegate const& response_handler, FString gas="300000000000000", FString deposit="0.000000000000000000000001");

	static void ContractView(FString const& contract_id, FFunctionCallData const& function_description, std::function<void(FString,bool)> const& response_handler);
	static void ContractCall(FString const& contract_id, FFunctionCallData const& function_description, FString const& wallet_authorization, std::function<void(FString,bool)> const& response_handler, FString gas="300000000000000", FString deposit="0.000000000000000000000001");

	// NFT - Enumeration
	// See NEP-181 at https://nomicon.io/Standards/Tokens/NonFungibleToken/Enumeration

	UFUNCTION(BlueprintCallable, Category="Nearlinker|NFT|Enumeration", meta=(AutoCreateRefTerm="contract_id"))
	static void ViewNftTotalSupply(FString const& contract_id, FNearHttpGetIntCompleteDelegate const& response_handler);
	UFUNCTION(BlueprintCallable, Category="Nearlinker|NFT|Enumeration", meta=(AutoCreateRefTerm="contract_id"))
	static void ViewNftTokens(FString const& contract_id, FNearHttpRequestCompleteDelegate const& response_handler, int from_index=0, int limit=100);
	UFUNCTION(BlueprintCallable, Category="Nearlinker|NFT|Enumeration", meta=(AutoCreateRefTerm="contract_id"))
	static void ViewNftSupplyForOwner(FString const& contract_id, FNearHttpGetIntCompleteDelegate const& response_handler, FString const& account_id);
	UFUNCTION(BlueprintCallable, Category="Nearlinker|NFT|Enumeration", meta=(AutoCreateRefTerm="contract_id"))
	static void ViewNftTokensForOwner(FString const& contract_id, FNearHttpRequestCompleteDelegate const& response_handler, FString const& account_id, int from_index=0, int limit=100);
};
