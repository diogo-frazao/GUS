// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GusCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerInteraction);

UCLASS()
class GUS_API AGusCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGusCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void AttachWeaponToHand(class AWeapon* WeaponToAttach) const;
	void DetachWeaponFromHand(const AWeapon* WeaponToDetach);
	
	/** Impulse the character when a weapon is shot into an "impulse" layer */
	void CheckWeaponFireImpulse(const FHitResult& HitResult, const AWeapon* WeaponWhoFired);

	UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
	void PlayFireEffects();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowCharacterMeshDelayed(float Delay, bool bShowMesh);

	/** Camera effects for when player is low HP */
	UFUNCTION(BlueprintImplementableEvent)
	void SetLowHPCameraEffects(bool bIsLowHP);

	FOnPlayerInteraction PlayerInteraction;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* CharacterMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* CombatComponent;

	/** Position where weapon will head towards when it hits an enemy */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponAttractionPoint = nullptr;

	/** Collider for detecting bullets damage */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BulletsHitArea = nullptr;

	void MoveForward(float Amount);
	void MoveRight(float Amount);
	void Turn(float Amount);
	void LookUp(float Amount);
	void CharacterJump();

	void FireHandgun();
	void ThrowWeapon();
	void Slide();
	bool CheckCanStopSliding() const;
	void StopSlide();

	UPROPERTY(Transient)
	APlayerController* PlayerController = nullptr;
	
	/** Camera Roll Effect */

	void RollCameraBasedOnRightMovement(float DeltaTime) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraRollSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraRollAmount = 10.f;

	float TargetCameraRoll = 0.f;

	/** Camera FOV Effect */

	void ChangeFOVBasedOnMovement(float DeltaTime) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float ChangeFOVSpeed = 8.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float ChangeFOVRunningAmount = 10.f;

	float DefaultCameraFOV = 90.f;
	float TargetCameraFOV = 90.f;

	/** Character Slide */

	/** Capsule height while sliding */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float SlideCapsuleHalfHeight = 35.f;

	/** Camera height while sliding */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float SlideCameraZOffset = 25.f;

	/** Multiplier to multiply current speed by while sliding. */
	/** The bigger the number the faster the character will come to rest.  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float SlideDeceleration = 8000.f;

	/** Multiplier to multiply current speed by while sliding */
	/** Values > 1 mean slide is faster than normal movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float SlidingSpeed = 2.f;

	/** Multiplier to multiply current speed by while sliding. */
	/** The bigger the number the faster the character will come to rest.  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LooseSlidingSpeedMultiplier = 3.f;

	/** 1 means full movement while sliding, 0 means none */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float SlidingMovementControl = 0.25f;

	const float SlideCameraRollSpeed = CameraRollSpeed * 4.f;
	float DefaultCameraOffsetZ = 64.f;
	float DefaultGroundFriction = 8.f;
	float DefaultBreakingDeceleration = 4000.f;
	float DefaultMaxWalkSpeed = 1200.f;
	float DefaultJumpVelocity = 1000.f;

	void SetCharacterSize(const float& CapsuleHalfHeight, const float& CameraZOffset) const;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsSliding = false;

	float CurrentSlidingSpeedMultiplier = 2.f;

	/** Health */

	bool bIsAlive = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	int MaxHealth = 2;

	int CurrentHealth = MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float TimeToRestoreHealth = 4.f;
	
	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Called after TimeToRestoreHealth when player takes damage */
	void RestorePlayerHealth();

	void RollCapsuleDeath(float DeltaTime, float TargetCapsuleRoll) const;

public:
	FORCEINLINE USkeletalMeshComponent* GetCharacterMesh() const { return CharacterMesh; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const {return CombatComponent; }
	FORCEINLINE float GetCurrentSlidingSpeedMultiplier() const { return CurrentSlidingSpeedMultiplier; }
	FORCEINLINE bool GetIsSliding() const { return bIsSliding; }
	FORCEINLINE FVector GetFireLocation() const { return CameraComponent->GetComponentLocation(); }
	FORCEINLINE FVector GetFireDirection() const { return CameraComponent->GetForwardVector(); }
	FORCEINLINE void CallShowCharacterMeshDelayed(float Delay, bool bShowMesh) { ShowCharacterMeshDelayed(Delay, bShowMesh); }
	FORCEINLINE USceneComponent* GetWeaponAttractionPoint() const { return WeaponAttractionPoint; }
	FORCEINLINE UBoxComponent* GetBulletsHitArea() const { return BulletsHitArea; }
	FORCEINLINE bool GetIsCharacterStop() const { return GetVelocity().Size() <= 200.f; }
};
