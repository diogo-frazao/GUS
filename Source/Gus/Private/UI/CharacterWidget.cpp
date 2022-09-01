// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterWidget.h"


void UCharacterWidget::RemoveBulletFromAmmoUI()
{
	if (AmmoUIArray.Num() == 0) { return; }
	
	UImage* ImageToRemove = AmmoUIArray[0];
	ImageToRemove->SetVisibility(ESlateVisibility::Hidden);
	AmmoUIArray.Remove(ImageToRemove);
}

void UCharacterWidget::CallRotateAmmoUI()
{
	RotateAmmoUI();
}
