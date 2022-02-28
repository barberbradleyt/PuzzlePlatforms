// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UServerResultWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    bool Selected = false;

    void Setup(class UMainMenu* InParent, uint32 InIndex);

    void SetSessionID(FString NewID);
    FString GetSessionID();

    void SetServerName(FString Name);
    FText GetServerName();

    void SetServerHostname(FString Hostname);
    FText GetServerHostname();

    void SetPlayerCount(FString PlayerCount);
    FText GetPlayerCount();

private:
    UPROPERTY(meta = (BindWidget))
    class UButton* ServerResultSelectButton;

    UPROPERTY()
    class UMainMenu* Parent;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ServerName;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ServerHostname;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayerCount;

    uint32 Index;

    uint32 MaxPlayers;

    uint32 CurrentPlayers;

    FString SessionID;

    UFUNCTION()
    void SelectServerResult();
};
