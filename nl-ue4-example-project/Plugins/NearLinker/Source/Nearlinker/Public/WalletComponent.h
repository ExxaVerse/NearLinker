// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#pragma once

#include"Components/ActorComponent.h" 

#include"WalletComponent.generated.h" 


DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FNearAddKeyDelegate, FString, url);
DECLARE_DYNAMIC_DELEGATE(FThenDelegate);

USTRUCT()
struct FNearKeyPair{
	GENERATED_BODY()

	UPROPERTY()
	FString public_key;
	UPROPERTY()
	FString private_key;
};

UCLASS(Blueprintable)
class NEARLINKER_API UNearlinkerWalletComponent: public UActorComponent{
	GENERATED_BODY()

//data
private:
	TArray<uint8> credentials;
public:
	UFUNCTION(BlueprintCallable, Category="Nearlinker")
	void SetCredentials(FString const& credentials_string);
	UFUNCTION(BlueprintCallable, Category="Nearlinker")
	bool LoadCredentials(FString const& file_path, FString const& password);
	UFUNCTION(BlueprintCallable, Category="Nearlinker")
	bool SaveCredentials(FString const& file_path, FString const& password);
	UFUNCTION(BlueprintCallable, Category="Nearlinker", meta=(AutoCreateRefTerm="ask_add_key,network,client_name"))
	void CreateCredentials(FString const& account, FThenDelegate const& then, FNearAddKeyDelegate const& ask_add_key, FString const& network="wallet.near.org", FString const& client_name="NearLinker");
	UFUNCTION(BlueprintCallable, Category="Nearlinker")
	FString GetAuthorizationForIntegrationServer();
};
