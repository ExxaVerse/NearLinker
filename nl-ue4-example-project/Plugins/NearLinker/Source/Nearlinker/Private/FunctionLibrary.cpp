// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#include"FunctionLibrary.h"
#include"Settings.h"
#include"log.h"
#include"Misc/MonitoredProcess.h"
#include"Runtime/Online/HTTP/Public/Http.h"
#include"Runtime/Online/SSL/Public/Ssl.h"
#include"Misc/Base64.h"
#include"JsonObjectConverter.h"

static TUniquePtr<FMonitoredProcess> IntegrationServerProcess;

void UNearlinkerFunctionLibrary::StartIntegrationServer(){
	auto const& server_conf=GetMutableDefault<UNearlinkerSettings>()->server_launch_settings;
	if(IntegrationServerProcess && IntegrationServerProcess->Update()){
		UE_LOG(LogNearlinker, Log, TEXT("Integration server is already running"));
	}else{
		IntegrationServerProcess=MakeUnique<FMonitoredProcess>(server_conf.executable_path, server_conf.args, server_conf.start_hidden);
		if(!IntegrationServerProcess){
			UE_LOG(LogNearlinker, Error, TEXT("Failed to allocate memory for the integration server FMonitoredProcess"));
			return;
		}
		IntegrationServerProcess->OnOutput().BindLambda([&](FString s){
			UE_LOG(LogNearlinker, Log, TEXT("Integration server: %s"), *s);
		});
		IntegrationServerProcess->OnCompleted().BindLambda([&](int32 status){
			UE_LOG(LogNearlinker, Log, TEXT("Integration server stopped with status %d"), status);
		});
		IntegrationServerProcess->Launch();
		if(IntegrationServerProcess->Update()){
			UE_LOG(LogNearlinker, Log, TEXT("Integration server started (%s %s)"), *server_conf.executable_path, *server_conf.args);
		}else{
			UE_LOG(LogNearlinker, Error, TEXT("Integration server failed to start (%s %s)"), *server_conf.executable_path, *server_conf.args);
		}
	}
}

void UNearlinkerFunctionLibrary::StopIntegrationServer(){
	if(!IntegrationServerProcess){
		UE_LOG(LogNearlinker, Log, TEXT("Integration server was not started"));
		return;
	}
	if(!IntegrationServerProcess->Update()){
		UE_LOG(LogNearlinker, Log, TEXT("Integration server is not running"));
		return;
	}
	IntegrationServerProcess->Cancel();
	UE_LOG(LogNearlinker, Log, TEXT("Integration server stopped"));
}

bool check_ssl(FString const& url){
	//Ensure HTTPS is used
	if(!url.StartsWith("https")){
		UE_LOG(LogNearlinker, Error, TEXT("%s is unsecure, please use https"), *url);
		return false;
	}
	//Optional: Ensure domain is pinned if it is localhost
	auto const domain=FPlatformHttp::GetUrlDomain(url);
	//if(domain==TEXT("localhost")){
		//if(!FSslModule::Get().GetCertificateManager().IsDomainPinned(domain)){
			//UE_LOG(LogNearlinker, Error, TEXT("%s is unsecure, please pin the SSL public key by adding the following  to your DefaultEngine.ini file:\n[SSL]\n+PinnedPublicKeys=\"%s:<Semicolon separated base64 encoded SHA256 digests of pinned public keys>\""), *url, *domain);
			//return false;
		//}
	//}
	UE_LOG(LogNearlinker, Verbose, TEXT("%s is secure (domain: %s)"), *url, *domain);
	return true;
}

void UNearlinkerFunctionLibrary::SendRequestToIntegrationServer(FString const& method, FString const& resource, FNearHttpRequestCompleteDelegate const& response_handler, FString const& data){
	return UNearlinkerFunctionLibrary::SendRequestToIntegrationServer(method, resource, [response_handler](FString s,bool b){
		if(!response_handler.ExecuteIfBound(s,b)){
			UE_LOG(LogNearlinker, Log, TEXT("No response handler"));
		}
	}, data);
}
void UNearlinkerFunctionLibrary::SendRequestToIntegrationServer(FString const& method, FString const& resource, std::function<void(FString,bool)> const& response_handler, FString const& data){
	auto const& server_url=GetMutableDefault<UNearlinkerSettings>()->server_url;
	//Log
	UE_LOG(LogNearlinker, Log, TEXT("Integration server %s request on %s"), *method, *(server_url+resource));
	if(data.Len()>0){
		UE_LOG(LogNearlinker, Log, TEXT("with data %s"), *data);
	}
	//Check that the connection is secure
	if(!check_ssl(server_url)){
		UE_LOG(LogNearlinker, Error, TEXT("Integration server is unsecure"));
#if !WITH_EDITOR
		//http is useful for debugging, so we keep it in development builds
		return
#endif
	}
	//Make request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(method);
	HttpRequest->SetURL(server_url+resource);
	HttpRequest->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	if(data.Len()>0){
		if(method==TEXT("GET")){
			UE_LOG(LogNearlinker, Error, TEXT("Integration server request should not include data when the method is GET"));
			return;
		}
		HttpRequest->SetHeader(TEXT("Content-Type"), "application/json");
		HttpRequest->SetContentAsString(data);
	}
	HttpRequest->OnProcessRequestComplete().BindLambda([response_handler](FHttpRequestPtr request, FHttpResponsePtr response, bool was_successful){
		UE_LOG(LogNearlinker, Log, TEXT("Integration server request complete"));
#if WITH_EDITOR
		// response might hold sensible information, so log it only in dev builds
		UE_LOG(LogNearlinker, Log, TEXT("%s"), *response->GetContentAsString());
#endif
		response_handler(response.IsValid()?response->GetContentAsString():FString{}, was_successful);
	});
	HttpRequest->ProcessRequest();
}

/*
void UNearlinkerFunctionLibrary::CreateWallet(FString const& wallet_name, FNearHttpRequestCompleteDelegate const& response_handler){
	UNearlinkerFunctionLibrary::SendRequestToIntegrationServer("PUT", FString{"/wallets/"}+wallet_name, response_handler);
}

void UNearlinkerFunctionLibrary::DeployContract(FNearContract const& contract, FString const& wallet_authorization, FNearHttpRequestCompleteDelegate const& response_handler){
	FString data_json;
	if(!FJsonObjectConverter::UStructToJsonObjectString(contract, data_json)){
		UE_LOG(LogNearlinker, Error, TEXT("Failed to export contract data to Json"));
		returna
	}
	UNearlinkerFunctionLibrary::SendRequestToIntegrationServer("POST", "/contract", response_handler, wallet_authorization, data_json);
}
*/

void UNearlinkerFunctionLibrary::ContractCall(FString const& contract_id, FFunctionCallData const& function_description, FString const& wallet_authorization, FNearHttpRequestCompleteDelegate const& response_handler, FString gas, FString deposit){
	FContractCallData data;
	wallet_authorization.Split(":", &data.account_id, &data.private_key);
	data.function=function_description;
	data.gas=gas;
	data.deposit=deposit;

	FString data_json;
	if(!FJsonObjectConverter::UStructToJsonObjectString(data, data_json)){
		UE_LOG(LogNearlinker, Error, TEXT("Failed to export contract function call data to Json"));
		return;
	}
	UNearlinkerFunctionLibrary::SendRequestToIntegrationServer("POST", FString{"/contract/"}+contract_id+FString{"/call"}, response_handler, data_json);
}
void UNearlinkerFunctionLibrary::ContractView(FString const& contract_id, FFunctionCallData const& function_description, FNearHttpRequestCompleteDelegate const& response_handler){
	FString function_description_string=function_description.name;
	if(function_description.parameters.Num()>0)                  function_description_string+="?";
	for(auto const& [key,value]:function_description.parameters) function_description_string+=key+"="+value+"&";
	function_description_string.RemoveFromEnd("&");
	UNearlinkerFunctionLibrary::SendRequestToIntegrationServer("GET", FString{"/contract"}/contract_id/function_description_string, response_handler);
}

