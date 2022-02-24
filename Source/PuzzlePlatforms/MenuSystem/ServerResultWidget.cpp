// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerResultWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "MainMenu.h"

void UServerResultWidget::SetSessionID(FString NewID) {
    SessionID = NewID;
}

FString UServerResultWidget::GetSessionID() {
    return SessionID;
}

void UServerResultWidget::SetServerName(FString Name) {
	ServerName->SetText(FText::FromString(Name));
}

FText UServerResultWidget::GetServerName() {
	return ServerName->GetText();
}

void UServerResultWidget::Setup(class UMainMenu* InParent, uint32 InIndex) {
	Parent = InParent;
	Index = InIndex;
	ServerResultSelectButton->OnClicked.AddDynamic(this, &UServerResultWidget::SelectServerResult);
}

void UServerResultWidget::SelectServerResult() {
	Parent->SelectIndex(Index);
}