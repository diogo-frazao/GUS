// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "GusCharacter.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Debug Sphere to see where enemy will start following the player
	FollowPlayerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("FollowPlayerSphere"));
	FollowPlayerSphere->SetupAttachment(RootComponent);
	FollowPlayerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	FollowPlayerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FollowPlayerSphere->SetSphereRadius(StartFollowingDistance);
	
	// Movement follows rotation
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Set player character reference
	PlayerCharacter = Cast<AGusCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	FollowPlayerSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnPlayerSphereOverlap);
	FollowPlayerSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnPlayerEndSphereOverlap);

	// Set enemy random walk speed
	const float DefaultEnemySpeed = GetCharacterMovement()->MaxWalkSpeed;
	const float RandomEnemySpeed = FMath::RandRange(DefaultEnemySpeed, DefaultEnemySpeed + SpeedThreshold);
	GetCharacterMovement()->MaxWalkSpeed = RandomEnemySpeed;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeSinceLastHit += DeltaTime;

	if (PlayerCharacter == nullptr) {return;}

	const float DistanceToPlayer = UKismetMathLibrary::Vector_Distance(GetActorLocation(), PlayerCharacter->GetActorLocation());
	if (DistanceToPlayer <= StartFollowingDistance)
	{
		TryGetNearPlayer();

		if (DistanceToPlayer <= StopDistance)
		{
			RotateTowardsPlayer();
		}
	}
}

void AEnemyCharacter::OnPlayerSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != PlayerCharacter) {return;}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AEnemyCharacter::OnPlayerEndSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != PlayerCharacter) {return;}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
}

void AEnemyCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	const AWeapon* OtherWeapon = Cast<AWeapon>(Other);
	if (OtherWeapon == nullptr || OtherWeapon->GetWasThrown() == false || !bCanReactToHits) { return; }
	
	// Called when a weapon was thrown and hits the enemy
	ImpulseWeaponToPlayer(OtherWeapon);
	EnemyHit(OtherWeapon->GetWeaponDamage(), Hit);
}

void AEnemyCharacter::ImpulseWeaponToPlayer(const AWeapon* WeaponToImpulse)
{
	if (!bCanReactToHits) { return; }

	UE_LOG(LogTemp, Warning, TEXT("Weapon thrown to player"));
	
	WeaponToImpulse->GetWeaponMesh()->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);

	PlayerCharacter = PlayerCharacter == nullptr ? Cast<AGusCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) : PlayerCharacter;

	FVector ImpulseDirection = PlayerCharacter->GetWeaponAttractionPoint()->GetComponentLocation() - GetActorLocation();
	ImpulseDirection *= WeaponImpulse;
	WeaponToImpulse->GetWeaponMesh()->AddForce(ImpulseDirection, TEXT(""), true);
	
	bCanReactToHits = false;

	// Enable weapon impulse to player after EnableHitReactionsTimer
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyCharacter::EnableWeaponImpulseToPlayer,
		EnableHitReactionsTimer, false);
}

void AEnemyCharacter::EnableWeaponImpulseToPlayer()
{
	bCanReactToHits = true;
}

void AEnemyCharacter::EnemyHit(const float& Damage, const FHitResult& HitResult)
{
	Health = FMath::Clamp(Health - Damage, 0.f, 999.f);

	if (Health == 0.f)
	{
		CallEnemyDeathReaction();
	}
	else
	{
		CallBulletHitReaction(HitResult);
		StartFollowingDistance = AfterHasSeenPlayerFollowingDistance;
	}
}




