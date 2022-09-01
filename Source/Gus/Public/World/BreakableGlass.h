// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDestructableInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "BreakableGlass.generated.h"

UCLASS()
class GUS_API ABreakableGlass : public AActor, public IIDestructableInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableGlass();
	
	UFUNCTION(BlueprintImplementableEvent)
	void DestroyGlassAndSlowdownTime();

	virtual void DestroyMesh_Implementation() override {}

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot = nullptr;
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BreakGlassTrigger = nullptr;

	/** Range from 0 to 1. 1 means normal speed and 0 the game is completely frozen */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SlowDownAmount = 0.25f;

	/** The amount of time the game will slow down when the player breaks the glass */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SlowDownTime = 4.f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bWasGlassBroken = false;

	/** Method called when player overlapped break glass trigger */
	UFUNCTION()
	void OnBreakGlassTriggerOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

public:
	void CallDestroyGlassAndSlowdownTime()
	{
		if (bWasGlassBroken) { return; }
		DestroyGlassAndSlowdownTime();
	};
};
