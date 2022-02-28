// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerResultWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "MainMenu.h"

//------------ Getters and Setters ------------//
//SessionID
void UServerResultWidget::SetSessionID(FString NewID) {
    SessionID = NewID;
}

FString UServerResultWidget::GetSessionID() {
    return SessionID;
}

//ServerName
void UServerResultWidget::SetServerName(FString Name) {
	ServerName->SetText(FText::FromString(Name));
}

FText UServerResultWidget::GetServerName() {
	return ServerName->GetText();
}

//ServerHostname
void UServerResultWidget::SetServerHostname(FString Hostname) {
	ServerHostname->SetText(FText::FromString(Hostname));
}

FText UServerResultWidget::GetServerHostname() {
	return ServerHostname->GetText();
}

//PlayerCount
void UServerResultWidget::SetPlayerCount(FString CountString) {
	PlayerCount->SetText(FText::FromString(CountString));
}

FText UServerResultWidget::GetPlayerCount() {
	return PlayerCount->GetText();
}
//---------------------------------------------//

void UServerResultWidget::Setup(class UMainMenu* InParent, uint32 InIndex) {
	Parent = InParent;
	Index = InIndex;
	ServerResultSelectButton->OnClicked.AddDynamic(this, &UServerResultWidget::SelectServerResult);
}

void UServerResultWidget::SelectServerResult() {
	Parent->SelectIndex(Index);
}