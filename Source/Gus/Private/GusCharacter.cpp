// Fill out your copyright notice in the Description page of Project Settings.


#include "GusCharacter.h"
#include "Macros.h"
#include "CombatComponent.h"
#include "GusGameModeBase.h"
#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AGusCharacter::AGusCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->SetRelativeLocation(FVector(0.f,0.f,64.f));
	CameraComponent->bUsePawnControlRotation = false;

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(CameraComponent);
	
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	WeaponAttractionPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponAttractionPoint"));
	WeaponAttractionPoint->SetupAttachment(CameraComponent);
	WeaponAttractionPoint->SetRelativeLocation(FVector(0.f,0.f,50.f));

	BulletsHitArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BulletsHitArea"));
	BulletsHitArea->SetupAttachment(CameraComponent);
	BulletsHitArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	BulletsHitArea->SetBoxExtent(FVector(1.f, 60.f, 32.f), false);
}

void AGusCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// Set default members (changed at runtime and use these as references to go back)
	DefaultCameraFOV = CameraComponent->FieldOfView;
	DefaultCameraOffsetZ = CameraComponent->GetRelativeLocation().Z;
	DefaultBreakingDeceleration = GetCharacterMovement()->BrakingDecelerationWalking;
	DefaultGroundFriction = GetCharacterMovement()->GroundFriction;
	DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultJumpVelocity = GetCharacterMovement()->JumpZVelocity;

	// Hide character hand's by default (no equipped weapon)
	CharacterMesh->SetHiddenInGame(true);

	CurrentSlidingSpeedMultiplier = SlidingSpeed;
}

void AGusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	check (CameraComponent != nullptr)

	// Roll Camera to side (visual feedback player is dead)
	if (bIsAlive == false)
	{
		GetMesh()->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);
		RollCapsuleDeath(DeltaTime, -60.f);
		return;
	}

	if (bIsSliding)
	{
		GetCharacterMovement()->MaxWalkSpeed = CurrentSlidingSpeedMultiplier;
		CurrentSlidingSpeedMultiplier = FMath::Clamp(CurrentSlidingSpeedMultiplier - DeltaTime * LooseSlidingSpeedMultiplier,
			0.f, SlidingSpeed);
	}

	RollCameraBasedOnRightMovement(DeltaTime);
	ChangeFOVBasedOnMovement(DeltaTime);
}

void AGusCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CombatComponent->Character = this;
}

void AGusCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGusCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGusCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AGusCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AGusCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AGusCharacter::CharacterJump);
	
	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &AGusCharacter::Slide);
	PlayerInputComponent->BindAction("Slide", IE_Released, this, &AGusCharacter::StopSlide);
	
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AGusCharacter::FireHandgun);
	PlayerInputComponent->BindAction("ThrowWeapon", IE_Pressed, this, &AGusCharacter::ThrowWeapon);
}

void AGusCharacter::MoveForward(float Amount)
{
	if (bIsAlive == false) { return; }
	
	// Change movement control if character is sliding or not.
	if (bIsSliding)
	{
		Amount = FMath::Clamp(Amount, -SlidingMovementControl, SlidingMovementControl);
		AddMovementInput(CameraComponent->GetForwardVector(), Amount);
	}
	else
	{
		AddMovementInput(CameraComponent->GetForwardVector(), Amount);
	}

	// Dynamic camera FOV based on movement
	if (FMath::Abs(Amount) > 0.f)
	{
		TargetCameraFOV = DefaultCameraFOV + ChangeFOVRunningAmount;
	}
	else
	{
		TargetCameraFOV = DefaultCameraFOV;
	}
}

void AGusCharacter::MoveRight(float Amount)
{
	if (bIsAlive == false) { return; }

	// Change movement control if character is sliding or not.
	if (bIsSliding)
	{
		Amount = FMath::Clamp(Amount, -SlidingMovementControl, SlidingMovementControl);
		AddMovementInput(CameraComponent->GetRightVector(), Amount);
	}
	else
	{
		AddMovementInput(CameraComponent->GetRightVector(), Amount);
	}
    
    // Only apply camera roll if character is grounded
    if (GetCharacterMovement()->IsFalling())
    {
    	TargetCameraRoll = 0.f;
	    return;
    }

	// Dynamic camera roll effect
	if (bIsSliding)
	{
		TargetCameraRoll = CameraRollAmount * 1.5f * -1.f;
	}
	else
	{
		if (Amount != 0.f)
		{
			TargetCameraRoll = Amount > 0.f ? -CameraRollAmount : CameraRollAmount;
		}
		else
		{
			TargetCameraRoll = 0.f;
		}	
	}
}

void AGusCharacter::Turn(float Amount)
{
	if (bIsAlive == false) { return; }

	CameraComponent->AddRelativeRotation(FRotator(0.f, Amount, 0.f));
}

void AGusCharacter::LookUp(float Amount)
{
	if (bIsAlive == false) { return; }

	if (CameraComponent->GetRelativeRotation().Pitch - Amount > -85.f &&
		CameraComponent->GetRelativeRotation().Pitch - Amount < 90.f)
	{
		CameraComponent->AddRelativeRotation(FRotator(Amount * -1.f, 0.f, 0.f));
	}
}

void AGusCharacter::CharacterJump()
{
	if (bIsAlive == false) { return; }

	if (bIsSliding && GetVelocity().Size() > 75.f)
	{
		GetCharacterMovement()->JumpZVelocity = DefaultJumpVelocity * 1.5f;
	}
	else
	{
		GetCharacterMovement()->JumpZVelocity = DefaultJumpVelocity;
	}
	Jump();
}

void AGusCharacter::FireHandgun()
{
	if (bIsAlive == false) { return; }

	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) {return;}

	CombatComponent->FireWeapon();
}

void AGusCharacter::ThrowWeapon()
{
	if (bIsAlive == false) { return; }

	PlayerInteraction.Broadcast();

	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) {return;}

	CombatComponent->ThrowWeapon();
}

void AGusCharacter::RollCameraBasedOnRightMovement(float DeltaTime) const
{
	const float CurrentRoll = CameraComponent->GetRelativeRotation().Roll;

	const float PitchClamped = FMath::Clamp(CameraComponent->GetRelativeRotation().Pitch, -85.f,
	                                        90.f);

	const float RollSpeed = bIsSliding ? SlideCameraRollSpeed : CameraRollSpeed;
	
	const FRotator TargetRotatorToSet(PitchClamped,
		CameraComponent->GetRelativeRotation().Yaw,
		FMath::Lerp(CurrentRoll, TargetCameraRoll, RollSpeed * DeltaTime));
	
	CameraComponent->SetRelativeRotation(TargetRotatorToSet);
}

void AGusCharacter::ChangeFOVBasedOnMovement(float DeltaTime) const
{
	const float CurrentFOV = CameraComponent->FieldOfView;
	const float LerpedTargetFOV = FMath::Lerp(CurrentFOV, TargetCameraFOV, ChangeFOVSpeed * DeltaTime);

	CameraComponent->FieldOfView = LerpedTargetFOV;
}

/** Slide */
void AGusCharacter::Slide()
{
	if (bIsAlive == false) { return; }

	bIsSliding = true;

	SetCharacterSize(SlideCapsuleHalfHeight,SlideCameraZOffset);
	
	GetCharacterMovement()->GroundFriction = 0.f;
	GetCharacterMovement()->BrakingDecelerationWalking = SlideDeceleration;
}

void AGusCharacter::StopSlide()
{
	if (CheckCanStopSliding() == false) { return; }
	
	bIsSliding = false;

	SetCharacterSize(88.f,64.f);

	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = DefaultBreakingDeceleration;

	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;

	CurrentSlidingSpeedMultiplier = SlidingSpeed;
}

bool AGusCharacter::CheckCanStopSliding() const
{
	if (GetCapsuleComponent() == nullptr) { return false; }

	const FVector FeetLocation = GetActorLocation() - FVector(0.f,0.f,GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	// 88.f is default capsule half height
	const FVector HeadLocation = FeetLocation + FVector(0.f,0.f,2 * 88.f);

	// Check if there's something between my feet and head
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, FeetLocation, HeadLocation, ECC_Visibility);

	#if DEBUG_ENABLED
		UKismetSystemLibrary::DrawDebugLine(this, FeetLocation, HeadLocation, FColor::Purple, 2.f, 4.f);
	#endif
	
	// If there's something, can't stop sliding
	return !HitResult.bBlockingHit;
}

void AGusCharacter::SetCharacterSize(const float& CapsuleHalfHeight, const float& CameraZOffset) const
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHalfHeight);
	const FVector CameraRelativeLocation = CameraComponent->GetRelativeLocation();
	CameraComponent->SetRelativeLocation(FVector(
		CameraRelativeLocation.X, CameraRelativeLocation.Y, CameraZOffset));
}

void AGusCharacter::AttachWeaponToHand(AWeapon* WeaponToAttach) const
{
	const FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget, true);
	WeaponToAttach->AttachToComponent(CharacterMesh, TransformRules,TEXT("WeaponSocket"));
}

void AGusCharacter::DetachWeaponFromHand(const AWeapon* WeaponToDetach)
{
	const FDetachmentTransformRules TransformRules(EDetachmentRule::KeepWorld,
	EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld, true);
	WeaponToDetach->GetRootComponent()->DetachFromComponent(TransformRules);
}

void AGusCharacter::CheckWeaponFireImpulse(const FHitResult& HitResult, const AWeapon* WeaponWhoFired)
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to impulse character"));
	
	// Can only be affected if is jumping
	if (!GetCharacterMovement()->IsFalling()) { return; }
	
	// Impulse character depending on surface's normal
	// On wall surface's, add Z velocity to allow player to bounce from walls to platforms.
	FVector SurfaceDirection = FVector::OneVector;
	
	// Surface is horizontal to the ground
	if (FMath::Abs(HitResult.ImpactNormal.Z) > 0.9f)
	{
		SurfaceDirection.Z = 0;
	}

	const bool bIsWallImpulse = SurfaceDirection.Z == 0.f ? false : true;
	
	// Direction to apply the impulse.
	const FVector ImpulseDirection = FVector(HitResult.ImpactNormal.X, HitResult.ImpactNormal.Y,
		bIsWallImpulse ? WeaponWhoFired->GetWallZImpulseMultiplier() : HitResult.ImpactNormal.Z);
	
	const float CharacterImpulse = bIsWallImpulse ? WeaponWhoFired->GetWallsImpulseForce() : WeaponWhoFired->GetGroundImpulseForce();
	
	LaunchCharacter(ImpulseDirection * CharacterImpulse, false, true);
}

float AGusCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f,  static_cast<float>(MaxHealth));

	if (CurrentHealth <= 0 && bIsAlive)
	{
		// Player death behavior
		bIsAlive = false;

		AGusGameModeBase* GameMode = Cast<AGusGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (GameMode == nullptr) { return 0.f; }
		GameMode->CallFadeOutAndRestartLevel();

		#if DEBUG_ENABLED
				if(GEngine)      GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, TEXT("Player Dead"));
		#endif
	}
	// Show player is low HP
	else
	{
		SetLowHPCameraEffects(true);

		#if DEBUG_ENABLED
			if(GEngine)      GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Player Damaged"));
		#endif
	}

	// Call restore health after X seconds
	FTimerHandle RestoreHealthTimerHandle;
	GetWorldTimerManager().SetTimer(RestoreHealthTimerHandle, this, &AGusCharacter::RestorePlayerHealth,
		TimeToRestoreHealth, false);
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AGusCharacter::RestorePlayerHealth()
{
	if (bIsAlive == false) { return; }
	
	CurrentHealth = MaxHealth;
	SetLowHPCameraEffects(false);
}

void AGusCharacter::RollCapsuleDeath(float DeltaTime, const float TargetCapsuleRoll) const
{
	const float CurrentRoll = GetCapsuleComponent()->GetRelativeRotation().Roll;
	
	const float RollSpeed = SlideCameraRollSpeed;
	
	const FRotator TargetRotatorToSet(0.f, 0.f,
		FMath::Lerp(CurrentRoll, TargetCapsuleRoll, RollSpeed * DeltaTime));
	
	GetCapsuleComponent()->SetRelativeRotation(TargetRotatorToSet);
}









