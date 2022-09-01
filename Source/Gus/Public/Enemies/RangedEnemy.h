// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "Components/BoxComponent.h"
#include "RangedEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GUS_API ARangedEnemy : public AEnemyCharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void RotateTowardsPlayer() override;
	virtual void CallEnemyDeathReaction() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> EnemyWeaponClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles = nullptr;

	/** Impulse applied upwards to the weapon when dropped */ 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float EnemyDropWeaponImpulse = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float TimeBetweenShots = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float FacePlayerRotationSpeed = 4.f;

	/** Fire accuracy. 1 means will always hit player and 0 will never hit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float FireAccuracy = 0.5f;

	/** Fire Accuracy will change to either more or less based on this threshold */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float FireAccuracyThreshold = 0.05f;

	/** If character is not moving, fire accuracy will be this amount */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float CharacterStopFireAccuracy = 0.65f;

	/** Visual threshold to move the point that player would receive the hit by */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float FailedShotDistance = 50.f;

	UFUNCTION(BlueprintCallable)
	void FireTowardsPlayer();

	/** Called when enemy dies */
	void DropWeapon() const;

	/** Used to check if there's something between the enemy and the player */
	bool IsTargetHiding(const FVector& StartPoint ,FVector& TargetPoint, const UWorld* World);

	/** Area used to choose a random point. Can either damage the player or not */
	UPROPERTY(Transient)
	UBoxComponent* PlayerHitArea = nullptr;

	/** Equipped weapon is created and attached on BeginPlay() */
	UPROPERTY(Transient)
	AWeapon* EquippedWeapon = nullptr;
};
