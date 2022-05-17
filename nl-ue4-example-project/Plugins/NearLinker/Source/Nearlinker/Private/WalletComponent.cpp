// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#include"WalletComponent.h"
#include"FunctionLibrary.h"
#include"log.h"
#include"Containers/UnrealString.h"
#include"Misc/FileHelper.h"
#include"Misc/AES.h"
#include"Runtime/Online/HTTP/Public/Http.h"
#include"JsonObjectConverter.h"

void UNearlinkerWalletComponent::SetCredentials(FString const& credentials_string){
	TArray<uint8> data;
	data.SetNum(credentials_string.Len()*sizeof(TCHAR));
	data.SetNum(StringToBytes(credentials_string, data.GetData(), data.Num()));

	this->credentials=data;

	UE_LOG(LogNearlinker, Log, TEXT("Credentials set (%d bytes)"), this->credentials.Num());

	//Debug
	//FString const credentials_string2=BytesToString(this->credentials.GetData(), this->credentials.Num());
	//UE_LOG(LogNearlinker, VeryVerbose, TEXT("In %s"), *credentials_string);
	//UE_LOG(LogNearlinker, VeryVerbose, TEXT("Set %s"), *credentials_string2);
}

bool UNearlinkerWalletComponent::SaveCredentials(FString const& file_path, FString const& password){
	TArray<uint8> data=this->credentials;
	//Adjust buffer size for AES
	uint32 RawSize=Align(data.Num(), FAES::AESBlockSize);
	if(uint32(data.Num())<RawSize){
		data.SetNumZeroed(RawSize);
	}
	//Encrypt
	auto key=FMD5::HashAnsiString(*password);
	FAES::EncryptData(data.GetData(), data.Num(), TCHAR_TO_ANSI(key.GetCharArray().GetData()));		
	//Save
	if(!FFileHelper::SaveArrayToFile(data, *file_path)){
		UE_LOG(LogNearlinker, Error, TEXT("SaveWalletFromFile: could not write to file %s"), *file_path);
		return false;
	}
	UE_LOG(LogNearlinker, Log, TEXT("Credentials saved to %s"), *file_path);
	return true;
}
bool UNearlinkerWalletComponent::LoadCredentials(FString const& file_path, FString const& password){
	TArray<uint8> data;
	if(!FFileHelper::LoadFileToArray(data, *file_path)){
		UE_LOG(LogNearlinker, Error, TEXT("LoadWalletFromFile: could not read file %s"), *file_path);
		return false;
	}
	if(data.Num()==0){
		UE_LOG(LogNearlinker, Error, TEXT("LoadWalletFromFile: file is empty"));
		return false;
	}
	auto key=FMD5::HashAnsiString(*password);
	FAES::DecryptData(data.GetData(), data.Num(), TCHAR_TO_ANSI(key.GetCharArray().GetData()));		
	credentials=data;
	return true;
}

void UNearlinkerWalletComponent::CreateCredentials(FString const& account, FThenDelegate const& then, FNearAddKeyDelegate const& ask_add_key, FString const& network, FString const& client_name){
	//For some reason, UE default inputs are cleared if passed by reference to lambdas
	UNearlinkerFunctionLibrary::SendRequestToIntegrationServer("GET", FString{"/keypair"}, [this,then,ask_add_key,account,network,client_name](FString response, bool request_succeded){
		if(!request_succeded){
			UE_LOG(LogNearlinker, Error, TEXT("CreateCredentials: failed to request a new key pair from the integration server"));
			return; 
		}
		FNearKeyPair key_pair;
		FJsonObjectConverter::JsonObjectStringToUStruct(response, &key_pair);
		
		FString const url_to_add_key="https://"+network+"/login/?referrer="+FPlatformHttp::UrlEncode(client_name)+"&public_key="+FPlatformHttp::UrlEncode(key_pair.public_key);
		UE_LOG(LogNearlinker, Log, TEXT("CreateCredentials: asking to add a new key at %s"), *url_to_add_key);
		bool key_was_added=[&](){
			if(ask_add_key.IsBound()){
				return ask_add_key.Execute(url_to_add_key);
			}
			FPlatformProcess::LaunchURL(*url_to_add_key, nullptr, nullptr);
			return true;
		}();
		if(key_was_added){
			this->SetCredentials(account+":"+key_pair.private_key);
		}
		if(!then.IsBound()){
			UE_LOG(LogNearlinker, Verbose, TEXT("CreateCredentials: then event is not bound"));
		}
		then.ExecuteIfBound();
	});
}

FString UNearlinkerWalletComponent::GetAuthorizationForIntegrationServer(){
	auto credentials_len=this->credentials.Find('\0');
	if(credentials_len==INDEX_NONE) credentials_len=this->credentials.Num();
	FString const credentials_string=BytesToString(this->credentials.GetData(), credentials_len);
	return credentials_string;
}
