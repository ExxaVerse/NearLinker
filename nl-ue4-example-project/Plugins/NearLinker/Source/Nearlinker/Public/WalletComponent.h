// Copyright 2022 ExxaVerse LLC. All Rights Reserved.

#pragma once

#include"Components/ActorComponent.h" 

#include"WalletComponent.generated.h" 


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
	UFUNCTION(BlueprintCallable, Category="Nearlinker")
	FString GetAuthorizationForIntegrationServer();
};
