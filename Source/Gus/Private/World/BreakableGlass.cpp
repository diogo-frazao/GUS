// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableGlass.h"

#include "GusCharacter.h"
#include "Kismet/GameplayStatics.h"

ABreakableGlass::ABreakableGlass()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneRoot);
	
	BreakGlassTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Break Glass Trigger"));
	BreakGlassTrigger->SetupAttachment(RootComponent);
	BreakGlassTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	BreakGlassTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ABreakableGlass::BeginPlay()
{
	Super::BeginPlay();

	BreakGlassTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABreakableGlass::OnBreakGlassTriggerOverlap);
	
}

void ABreakableGlass::OnBreakGlassTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const AGusCharacter* OtherCharacter = Cast<AGusCharacter>(OtherActor);
	if (OtherCharacter == nullptr || bWasGlassBroken) { return; }

	CallDestroyGlassAndSlowdownTime();
}

