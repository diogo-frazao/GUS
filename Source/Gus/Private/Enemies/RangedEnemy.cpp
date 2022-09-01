// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedEnemy.h"
#include "Macros.h"
#include "GusCharacter.h"
#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"


void ARangedEnemy::BeginPlay()
{
	Super::BeginPlay();

	EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(EnemyWeaponClass, GetActorLocation(), FRotator::ZeroRotator);
	if (EquippedWeapon == nullptr) { return; }

	EquippedWeapon->SetIsEnemyWeapon(true);
	EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SOCKET_Hand_R"));
	EquippedWeapon->SetActorRelativeLocation(FVector(0.23f, -0.51f, 0.08f), false, nullptr, ETeleportType::TeleportPhysics);
	EquippedWeapon->SetActorRelativeRotation(FRotator(-11.97f, 13.97f, -1.15f), false, nullptr, ETeleportType::TeleportPhysics);

	FireAccuracy += FMath::RandRange(-FireAccuracyThreshold, FireAccuracyThreshold);
}

void ARangedEnemy::FireTowardsPlayer()
{
	if (PlayerCharacter == nullptr || Health <= 0.f || EquippedWeapon == nullptr) { return; }

	UWorld* World = GetWorld();
	if (World == nullptr) { return; }
	
	PlayerHitArea = PlayerHitArea == nullptr ? PlayerCharacter->GetBulletsHitArea() : PlayerHitArea;

	// Get random point inside player hit area
	FVector RandomPointToShootPlayer = UKismetMathLibrary::RandomPointInBoundingBox(
		PlayerHitArea->GetComponentLocation(), PlayerHitArea->GetScaledBoxExtent());
	
	const float FireChance = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);

	// Increase fire accuracy if player is not moving
	const float CurrentFireAccuracy = PlayerCharacter->GetIsCharacterStop() ? CharacterStopFireAccuracy : FireAccuracy;

	const FString CurrentFireAccuracyString = FString::SanitizeFloat(CurrentFireAccuracy);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Silver, *CurrentFireAccuracyString);

	const bool bIsPlayerHiding = IsTargetHiding(EquippedWeapon->GetFireSpot(),
		RandomPointToShootPlayer, World);

	#if DEBUG_ENABLED
		UKismetSystemLibrary::DrawDebugLine(World, EquippedWeapon->GetFireSpot(), RandomPointToShootPlayer,
			FColor::Magenta, 2.f, 4.f);
	#endif
	
	if (CurrentFireAccuracy >= FireChance && bIsPlayerHiding == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Enemy Hit Player"));
		
		UGameplayStatics::ApplyDamage(PlayerCharacter, 1.f, GetController(),
			this, UDamageType::StaticClass());

		EquippedWeapon->WeaponFireEffects(RandomPointToShootPlayer, World, false);

		#if DEBUG_ENABLED
				UKismetSystemLibrary::DrawDebugLine(GetWorld(), GetActorLocation(), RandomPointToShootPlayer, FColor::Cyan, 5.f, 5.f);
		#endif
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Enemy Failed to Hit Player"));

		// Change fire point to show enemy failed to hit player
		const bool bMissShotToTheRight = FMath::RandBool();
		if (bMissShotToTheRight)
		{
			RandomPointToShootPlayer += PlayerHitArea->GetRightVector() * FailedShotDistance;
		}
		else
		{
			RandomPointToShootPlayer += PlayerHitArea->GetRightVector() * -FailedShotDistance;
		}
		
		EquippedWeapon->WeaponFireEffects(RandomPointToShootPlayer, World, false);
	}
}

bool ARangedEnemy::IsTargetHiding(const FVector& StartPoint ,FVector& TargetPoint, const UWorld* World)
{
	FHitResult HitResult;
	World->LineTraceSingleByChannel(HitResult, StartPoint, TargetPoint, ECollisionChannel::ECC_Visibility);
	
	if (HitResult.bBlockingHit)
	{
		TargetPoint = HitResult.ImpactPoint;
		return true;
	}
	return  false;
}

void ARangedEnemy::RotateTowardsPlayer()
{
	if (PlayerCharacter == nullptr || Health <= 0.f) { return; }

	const FVector Start = GetActorLocation();
	FVector Target = PlayerCharacter->GetActorLocation();
	Target.Z = Start.Z;

	const FRotator LookAtPlayerRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	
	const FRotator TargetRotation = UKismetMathLibrary::RLerp(GetActorRotation(),
		LookAtPlayerRotation, FacePlayerRotationSpeed, true);

	SetActorRotation(TargetRotation);
}

// Called when enemy dies
void ARangedEnemy::CallEnemyDeathReaction()
{
	DropWeapon();
	
	Super::CallEnemyDeathReaction();
}

void ARangedEnemy::DropWeapon() const
{
	if (EquippedWeapon == nullptr) { return; }

	GetMesh()->IgnoreActorWhenMoving(EquippedWeapon, true);
	GetCapsuleComponent()->IgnoreActorWhenMoving(EquippedWeapon, true);
	
	const FDetachmentTransformRules TransformRules(EDetachmentRule::KeepWorld,
EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld, true);
	EquippedWeapon->GetRootComponent()->DetachFromComponent(TransformRules);
	
	EquippedWeapon->GetWeaponMesh()->SetSimulatePhysics(true);
	
	EquippedWeapon->GetWeaponMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	EquippedWeapon->GetWeaponMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	EquippedWeapon->SetIsEnemyWeapon(false);

	EquippedWeapon->GetWeaponMesh()->AddImpulse(FVector::UpVector * EnemyDropWeaponImpulse, TEXT(""), true);
}
