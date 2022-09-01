// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "CharacterWidget.h"
#include "CombatComponent.h"
#include "DrawDebugHelpers.h"
#include "EnemyCharacter.h"
#include "GusCharacter.h"
#include "IDestructableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Macros.h"
#include "Blueprint/UserWidget.h"
#include "Misc/TextFilterExpressionEvaluator.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionResponseToChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetSimulatePhysics(true);

	WeaponCollision = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponCollision"));
	WeaponCollision->SetupAttachment(RootComponent);
	WeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	ThrowWeaponDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ThrowWeaponDirectionArrow"));
	ThrowWeaponDirectionArrow->SetupAttachment(RootComponent);

	GetWeaponMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponOverlap);
	WeaponCollision->SetVisibility(false);
	ThrowWeaponDirectionArrow->SetVisibility(false);

	WeaponMesh->SetPhysicsMaxAngularVelocityInDegrees(MaxAngularVelocityPerSecond);

	// Ignore collision with player (just for the mesh not the collision sphere) 
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter == nullptr) {return;} 
	WeaponMesh->IgnoreActorWhenMoving(PlayerCharacter, true);

	CurrentAmmo = WeaponAmmo;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsEnemyWeapon) { return; }
	
	// If is not overlapping with player character, return
	AGusCharacter* PlayerCharacter = Cast<AGusCharacter>(UGameplayStatics::GetPlayerCharacter(
		this, 0));
	
	if (OtherActor != PlayerCharacter || PlayerCharacter->GetCombatComponent()->GetEquippedWeapon() != nullptr) {return;}

	PlayerCharacter->GetCombatComponent()->EquipWeapon(this);
	SetEquippedWeaponProperties(true, PlayerCharacter);

	// If weapon is empty by default, don't show weapon HUD (same as 0 bullets)
	if (CurrentAmmo > 0)
	{
		ShowAmmoUI(true);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s has %i bullets"), *GetName(), CurrentAmmo);
}

void AWeapon::WeaponFire(const FVector& FireLocation, const FVector& FireDirection)
{
	UWorld* World = GetWorld();
	if (World == nullptr) {return;}

	// Check for out of bullets
	if (CurrentAmmo == 0) { return; }

	

	// Trace straight from the camera	
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	
	FVector FireEnd = FireLocation + FireDirection * WeaponRange;
	World->LineTraceSingleByChannel(HitResult, FireLocation,
		FireEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
	
	FVector BeamEnd = FireEnd;
	
	if (HitResult.bBlockingHit)
	{
		#if DEBUG_ENABLED
				DrawDebugBox(World, HitResult.ImpactPoint, FVector::OneVector * 2.f, FColor::Red, false, 4.f, 0.f, 4.f);
		#endif
		
		BeamEnd = HitResult.ImpactPoint;
		
		BulletHitEffects(HitResult);
		
		AActor* HitActor = HitResult.GetActor();
		
		// Check fire impulse
		if (HitActor && HitActor->ActorHasTag(TEXT("Impulse")) && HitResult.Distance <= ImpulseRange)
		{
			AGusCharacter* OwnerCharacter = Cast<AGusCharacter>(GetOwner());
			if (OwnerCharacter == nullptr) { return; }
			OwnerCharacter->CheckWeaponFireImpulse(HitResult, this);
		#if DEBUG_ENABLED
			DrawDebugLine(World, FireLocation, FireEnd, FColor::Orange, false, 5.f);
		#endif
		}

		// Check fire hit enemy
		AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(HitResult.GetActor());
		if (EnemyCharacter)
		{
			EnemyCharacter->EnemyHit(BulletDamage, HitResult);
			UE_LOG(LogTemp, Warning, TEXT("Enemy hit reaction to weapon fire"));
		}

		// Check Destructible Mesh hit
		IIDestructableInterface* DestructibleInterface = Cast<IIDestructableInterface>(HitResult.GetActor());
		if (DestructibleInterface)
		{
			DestructibleInterface->Execute_DestroyMesh(HitResult.GetActor());
		}
	}
	WeaponFireEffects(BeamEnd, World, true);
}

void AWeapon::WeaponFireEffects(const FVector& BeamEnd, UWorld* World, const bool bRemoveBulletFromWeapon)
{
	// Play Fire Sound
	PlayBulletFireSound();
	
	// Spawn muzzle flash
	UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, WeaponMesh, TEXT("SOCKET_Muzzle"),
		FVector::ZeroVector,FRotator(60.f, -10.f, 0.f),
		FVector(0.65f, 0.65f, 0.65f), EAttachLocation::KeepRelativeOffset, true);

	// Spawn Beam Particles
	SpawnBeamParticles(BeamEnd, World);

	if (bRemoveBulletFromWeapon)
	{
		// Remove ammo from weapon
		CurrentAmmo = FMath::Clamp(CurrentAmmo - 1, 0, WeaponAmmo);

		ShowAmmoUI(true);
		if (AmmoWidget == nullptr) { return; }

		// Remove bullet from ammo UI
		AmmoWidget->RemoveBulletFromAmmoUI();

		// Call Rotate Ammo UI
		AmmoWidget->RotateAmmoUI();	
	}
}

void AWeapon::SpawnBeamParticles(const FVector& BeamEnd, UWorld* World) const
{
	const FTransform MuzzleSocketTransform = WeaponMesh->GetSocketTransform(TEXT("SOCKET_Muzzle"),
		ERelativeTransformSpace::RTS_World);

#if DEBUG_ENABLED
	DrawDebugBox(World, MuzzleSocketTransform.GetLocation(), FVector::OneVector * 2.f, FColor::Green, false, 4.f, 0.f, 4.f);
#endif

	if (!MuzzleSocketTransform.IsValid()) { return; }

	if (BeamParticles == nullptr) { return; }
	UParticleSystemComponent* BeamParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(
		World, BeamParticles, MuzzleSocketTransform);

	if (BeamParticleComponent == nullptr) { return; }
	BeamParticleComponent->SetVectorParameter(FName("Target"), BeamEnd);
}

void AWeapon::ShowAmmoUI(const bool& Value)
{
	AmmoWidget = AmmoWidget == nullptr ? Cast<UCharacterWidget>(CreateWidget(UGameplayStatics::GetPlayerController(
		this, 0), AmmoWidgetClass)) : AmmoWidget;
	
	if (AmmoWidget == nullptr) { return; }

	if (Value)
	{
		if (AmmoWidget ->IsInViewport()) { return; }
		AmmoWidget->AddToViewport();
	}
	else
	{
		AmmoWidget->RemoveFromParent();
	}
}

void AWeapon::WeaponThrow(const FVector& ThrowDirection)
{
	if (GetOwner() == nullptr) { return; }
	AGusCharacter* PlayerCharacter = Cast<AGusCharacter>(GetOwner());

	PlayThrowWeaponSound();

	GetWeaponMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	
	SetEquippedWeaponProperties(false, PlayerCharacter);
	
	const FVector ForceToApply = ThrowWeaponDirectionArrow->GetForwardVector() * ThrowForce;
	WeaponMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);
	WeaponMesh->AddForceAtLocation(ForceToApply, ThrowWeaponDirectionArrow->GetComponentLocation());

	bWasThrown = true;
	
	EnableWeaponCollisionWithPlayerAfter(1.f, true);
}

void AWeapon::SetEquippedWeaponProperties(const bool& bIsEquippingWeapon, AGusCharacter* PlayerCharacter)
{
	if (bIsEquippingWeapon)
	{
		// Disable physics and attach weapon to hand socket
		PlayerCharacter->AttachWeaponToHand(this);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetSimulatePhysics(false);
		SetOwner(PlayerCharacter);
	}
	else
	{
		// Enable physics and detach weapon to hand socket
		PlayerCharacter->DetachWeaponFromHand(this);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetSimulatePhysics(true);
		ShowAmmoUI(false);
		SetOwner(nullptr);
	}
}








