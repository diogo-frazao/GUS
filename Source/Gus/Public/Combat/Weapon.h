// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class GUS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	
	void WeaponFire(const FVector& FireLocation, const FVector& FireDirection);
	void WeaponThrow(const FVector& ThrowDirection);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayBulletFireSound();
	
	UFUNCTION(BlueprintImplementableEvent)
	void PlayThrowWeaponSound();

	UFUNCTION(BlueprintImplementableEvent)
	void BulletHitEffects(const FHitResult& HitResult);

	UFUNCTION(BlueprintImplementableEvent)
	void EnableWeaponCollisionWithPlayerAfter(float DelayTime, bool bEnable);
	
	UFUNCTION(BlueprintPure) const
	class UCharacterWidget* GetAmmoWidget() const { return AmmoWidget; }

	void WeaponFireEffects(const FVector& BeamEnd, UWorld* World, const bool bRemoveBulletFromWeapon);
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh = nullptr;

	/** Whether this weapon starts attached to an enemy or not */ 
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsEnemyWeapon = false;

	/** Maximum amount of degrees the weapon can rotate per second when thrown */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxAngularVelocityPerSecond = 80.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* WeaponCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* ThrowWeaponDirectionArrow = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WeaponRange = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ThrowForce = 4000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 WeaponAmmo = 4;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 CurrentAmmo = WeaponAmmo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire Impulse", meta = (AllowPrivateAccess = "true"))
	float ImpulseRange = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire Impulse", meta = (AllowPrivateAccess = "true"))
	float GroundImpulseForce = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire Impulse", meta = (AllowPrivateAccess = "true"))
	float WallsImpulseForce = 4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire Impulse", meta = (AllowPrivateAccess = "true"))
	float WallZImpulseMultiplier = 400.f;

	/** Damage */
	
	/** Damage the weapon deals when is thrown against an enemy */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	int WeaponDamage = 2;

	/** Damage the weapon deals when fired against an enemy */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	int BulletDamage = 1;

	bool bWasThrown = false;

	/** Ammo UI */
	
	/** Widget to display when this weapon is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCharacterWidget> AmmoWidgetClass = nullptr;

	UPROPERTY(Transient)
	UCharacterWidget* AmmoWidget = nullptr;

	void ShowAmmoUI(const bool& Value);
	void SetEquippedWeaponProperties(const bool& bIsEquippingWeapon, class AGusCharacter* PlayerCharacter);

	/** Method called when player overlapped with weapon collection sphere */
	UFUNCTION()
	virtual void OnWeaponOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void SpawnBeamParticles(const FVector& BeamEnd, UWorld* World) const;
public:
	FORCEINLINE UParticleSystem* GetMuzzleParticle() const { return MuzzleParticle; }
	FORCEINLINE bool CanFire() const { return CurrentAmmo > 0; }
	FORCEINLINE bool CanThrowWeapon() const { return CurrentAmmo == 0 && GetNetOwner() != nullptr; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetImpulseRadius() const { return ImpulseRange; }
	FORCEINLINE float GetGroundImpulseForce() const { return GroundImpulseForce; }
	FORCEINLINE float GetWallsImpulseForce() const { return WallsImpulseForce; }
	FORCEINLINE float GetWallZImpulseMultiplier() const { return WallZImpulseMultiplier;}
	FORCEINLINE int GetWeaponDamage() const { return WeaponDamage;}
	FORCEINLINE bool GetWasThrown() const { return bWasThrown; }
	FORCEINLINE void SetIsEnemyWeapon(const bool Value) { bIsEnemyWeapon = Value; }
	FORCEINLINE FVector GetFireSpot() const { return WeaponMesh->GetSocketLocation(TEXT("SOCKET_Muzzle")); }
};
