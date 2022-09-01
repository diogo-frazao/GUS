// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class GUS_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void BulletHitReaction(const FHitResult& HitResult);
	
    UFUNCTION(BlueprintImplementableEvent)
	void WeaponThrowHitReaction();

	UFUNCTION(BlueprintImplementableEvent)
	void TryGetNearPlayer();
	
	void EnemyHit(const float& Damage, const FHitResult& HitResult);
	
protected:
	
	virtual void BeginPlay() override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/** Implement in child classes in which AI must always face player when in range */
	virtual void RotateTowardsPlayer(){};

	virtual void CallEnemyDeathReaction() { WeaponThrowHitReaction(); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* FollowPlayerSphere = nullptr;
	
	/** Time since last time the player was hit by a weapon throw or weapon fire */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float TimeSinceLastHit = 1.f;
	
	/** Impulse Weapon to Player On Hit */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Impulse", meta = (AllowPrivateAccess = "true"))
	float EnableHitReactionsTimer = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Impulse", meta = (AllowPrivateAccess = "true"))
	float WeaponImpulse = 100.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AGusCharacter* PlayerCharacter = nullptr;

	/** Enemy Health */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int Health = 2;

	/** Enemy AI */
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float AfterHasSeenPlayerFollowingDistance = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float StartFollowingDistance = 400.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float StopDistance = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SpeedThreshold = 100.f;

	/** Method called when player overlapped with following sphere */
	UFUNCTION()
	virtual void OnPlayerSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/** Method called when player stops overlapping with following sphere */
	UFUNCTION()
	virtual void OnPlayerEndSphereOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
	
	bool bCanReactToHits = true;
	void EnableWeaponImpulseToPlayer();
	void ImpulseWeaponToPlayer(const class AWeapon* WeaponToImpulse);

public:
	FORCEINLINE void CallBulletHitReaction(const FHitResult& HitResult) { BulletHitReaction(HitResult); }
};
