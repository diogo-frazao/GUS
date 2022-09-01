// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeEnemy.h"
#include "Macros.h"
#include "GusCharacter.h"
#include "Kismet/GameplayStatics.h"

AMeleeEnemy::AMeleeEnemy()
{
	DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageSphere"));
	DamageSphere->SetupAttachment(GetMesh(), TEXT("hand_r"));
	
	DamageSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	DamageSphere->SetSphereRadius(AttackRange);
	DamageSphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnPlayerDamageSphereOverlap);
}

void AMeleeEnemy::EnableDamageSphere(const bool Value)
{
	if (Value)
	{
		DamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		#if DEBUG_ENABLED
				DamageSphere->SetHiddenInGame(false);
		#endif
	}
	else
	{
		DamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

		#if DEBUG_ENABLED
				DamageSphere->SetHiddenInGame(true);
		#endif
	}
}

// Called in the frame of the animation where the melee enemy delivers the punch
void AMeleeEnemy::OnPlayerDamageSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Health <= 0) { return; }
	
	const AGusCharacter* OtherCharacter = Cast<AGusCharacter>(OtherActor);
	if (OtherCharacter == nullptr) {return;}

	AController* MyController = GetController();
	if (MyController == nullptr) { return; }

	// Play Punch Sound
	PlayEnemyPunchSound();
	
	// Damage Player
	UGameplayStatics::ApplyDamage(OtherActor, 1.f, MyController,
		this, UDamageType::StaticClass());
}
