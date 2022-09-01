// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "CharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUS_API UCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* CrosshairCanvas = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* AmmoTop = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* AmmoRight = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* AmmoLeft = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* AmmoDown = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TArray<UImage*> AmmoUIArray;

	UFUNCTION(BlueprintCallable)
	void RemoveBulletFromAmmoUI();
	
	UFUNCTION(BlueprintCallable)
	void CallRotateAmmoUI();

	UFUNCTION(BlueprintImplementableEvent)
	void RotateAmmoUI();
};
