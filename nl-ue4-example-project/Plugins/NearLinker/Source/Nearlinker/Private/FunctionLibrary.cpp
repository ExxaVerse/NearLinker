// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#include"FunctionLibrary.h"
#include"Settings.h"
#include"log.h"
#include"Misc/MonitoredProcess.h"
#include"Runtime/Online/HTTP/Public/Http.h"
#include"Misc/Base64.h"

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

void UNearlinkerFunctionLibrary::SendRequestToIntegrationServer(FString const& wallet_authorization, FString const& method, FString const& resource, FString const& data){
	UE_LOG(LogNearlinker, Log, TEXT("Integration server request on %s"), *(GetMutableDefault<UNearlinkerSettings>()->server_url+resource));
	if(!GetMutableDefault<UNearlinkerSettings>()->server_url.StartsWith("https")){
		UE_LOG(LogNearlinker, Error, TEXT("Integration server request is unsecure, please use https"));
#if !WITH_EDITOR
		//http is useful for debugging, so we keep it in development builds
		return;
#endif
	}
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(method);
	HttpRequest->SetURL(GetMutableDefault<UNearlinkerSettings>()->server_url+resource);
	HttpRequest->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	HttpRequest->SetHeader(TEXT("Authorization"), FString{TEXT("Basic ")}+FBase64::Encode(wallet_authorization));
	if(method==TEXT("POST")){
		HttpRequest->SetHeader(TEXT("Content-Type"), "application/json");
		HttpRequest->SetContentAsString(data);
	}
	HttpRequest->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr request, FHttpResponsePtr response, bool was_successful){
		UE_LOG(LogNearlinker, Log, TEXT("Integration server request complete"));
		if(was_successful && response.IsValid()){
			UE_LOG(LogNearlinker, Log, TEXT("%s"), *response->GetContentAsString());
		}
	});
	HttpRequest->ProcessRequest();
}

