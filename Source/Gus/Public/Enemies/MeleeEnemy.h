// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "MeleeEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GUS_API AMeleeEnemy : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	AMeleeEnemy();

	UFUNCTION(BlueprintCallable)
	void EnableDamageSphere(bool Value);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayEnemyPunchSound();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* DamageSphere = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float AttackRange = 250.f;

	/** Method called when player overlaps with damage sphere */
	UFUNCTION()
	virtual void OnPlayerDamageSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
