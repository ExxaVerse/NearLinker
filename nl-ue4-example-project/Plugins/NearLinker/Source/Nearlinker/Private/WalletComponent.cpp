// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#include"WalletComponent.h"
#include"log.h"
#include"Containers/UnrealString.h"
#include"Misc/FileHelper.h"
#include"RSA.h"

void UNearlinkerWalletComponent::SetCredentials(FString const& credentials_string){
	TArray<uint8> data;
	data.SetNum(credentials_string.Len()*sizeof(TCHAR));
	data.SetNum(StringToBytes(credentials_string, data.GetData(), data.Num()));

	//FRSA::EncryptPublic(data, this->credentials, rsa_key);
	UE_LOG(LogNearlinker, Error, TEXT("TODO: encrypt credentials in plugin RAM"));
	this->credentials=data;

	UE_LOG(LogNearlinker, Log, TEXT("Credentials set (%d bytes)"), this->credentials.Num());

	//Debug
	//FString const credentials_string2=BytesToString(this->credentials.GetData(), this->credentials.Num());
	//UE_LOG(LogNearlinker, VeryVerbose, TEXT("In %s"), *credentials_string);
	//UE_LOG(LogNearlinker, VeryVerbose, TEXT("Set %s"), *credentials_string2);
}

bool UNearlinkerWalletComponent::SaveCredentials(FString const& file_path, FString const& password){
	FText error;
	if(!FFileHelper::IsFilenameValidForSaving(file_path, error)){
		UE_LOG(LogNearlinker, Error, TEXT("LoadWalletFromFile: invalid filename"));
		return false;
	}
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
		UE_LOG(LogNearlinker, Error, TEXT("LoadWalletFromFile: could not write to file %s"), *file_path);
		return false;
	}
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

FString UNearlinkerWalletComponent::GetAuthorizationForIntegrationServer(){
	FString const credentials_string=BytesToString(this->credentials.GetData(), this->credentials.Num());
	return credentials_string;
}
