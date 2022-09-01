// Fill out your copyright notice in the Description page of Project Settings.


#include "FinishButton.h"

#include "GusCharacter.h"
#include "GusGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AFinishButton::AFinishButton()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	ButtonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Button Mesh"));
	ButtonMesh->SetupAttachment(RootComponent);
	ButtonMesh->SetRelativeScale3D(FVector(0.75f,0.75f,0.75f));
	ButtonMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	ButtonCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Button Collision Box"));
	ButtonCollisionBox->SetupAttachment(RootComponent);
	ButtonCollisionBox->SetCollisionResponseToAllChannels(ECR_Block);

	ButtonInteractionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Button Interaction Area"));
	ButtonInteractionArea->SetupAttachment(RootComponent);
	ButtonInteractionArea->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void AFinishButton::BeginPlay()
{
	Super::BeginPlay();

	ButtonInteractionArea->OnComponentBeginOverlap.AddDynamic(this, &AFinishButton::OnButtonOverlap);
	ButtonInteractionArea->OnComponentEndOverlap.AddDynamic(this, &AFinishButton::OnButtonEndOverlap);

	PlayerCharacter = Cast<AGusCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacter == nullptr) { return; }

	PlayerCharacter->PlayerInteraction.AddDynamic(this, &AFinishButton::OnPlayerInteraction);
}

void AFinishButton::OnButtonOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bIsPlayerInsideInteractionArea = true;
	ShowButtonEnabledEffect(true);
}

void AFinishButton::OnButtonEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsPlayerInsideInteractionArea = false;
	ShowButtonEnabledEffect(false);
}

void AFinishButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFinishButton::OnPlayerInteraction()
{
	if (bIsPlayerInsideInteractionArea == false) { return; }

	PlayButtonPressedSound();

	UE_LOG(LogTemp, Warning, TEXT("Player interacted with button"));
	
	UGameplayStatics::SetGlobalTimeDilation(this, 0.25f);

	if (ButtonPullAnimation == nullptr) { return; }
	ButtonMesh->PlayAnimation(ButtonPullAnimation, false);
	
	AGusGameModeBase* GameMode = Cast<AGusGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode == nullptr) { return; }
	GameMode->CallFadeOutAndGoToNextLevel();
}


