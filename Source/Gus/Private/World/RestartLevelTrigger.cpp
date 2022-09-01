// Fill out your copyright notice in the Description page of Project Settings.


#include "RestartLevelTrigger.h"

#include "GusCharacter.h"
#include "GusGameModeBase.h"
#include "Kismet/GameplayStatics.h"

ARestartLevelTrigger::ARestartLevelTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RestartCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RestartCollision"));
	SetRootComponent(RestartCollision);
	RestartCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	RestartCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ARestartLevelTrigger::BeginPlay()
{
	Super::BeginPlay();

	RestartCollision->OnComponentBeginOverlap.AddDynamic(this, &ARestartLevelTrigger::OnRestartCollisionOverlap);
}

void ARestartLevelTrigger::OnRestartCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("restart level trigger detected"));
	
	const AGusCharacter* OtherCharacter = Cast<AGusCharacter>(OtherActor);
	if (OtherCharacter == nullptr) { return; }

	AGusGameModeBase* GameMode = Cast<AGusGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode == nullptr) { return; }
	
	GameMode->CallFadeOutAndRestartLevel();
}




