// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugHUD.h"

#include "CombatComponent.h"
#include "GusCharacter.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ADebugHUD::ADebugHUD()
{
	static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Engine/EngineFonts/Roboto"));

	MainFont = Font.Object;
}

void ADebugHUD::DrawHUD()
{
	Super::DrawHUD();
	
	X = Y = 50.0f;

	const AGusCharacter* PlayerCharacter = Cast<AGusCharacter>(GetOwningPawn());
	check (PlayerCharacter != nullptr)

	AddFloat(L"Ground friction", PlayerCharacter->GetCharacterMovement()->GroundFriction);
	AddFloat(L"Breaking Deceleration", PlayerCharacter->GetCharacterMovement()->BrakingDecelerationWalking);
	AddFloat(L"Character Speed", PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed);
	AddBool(L"Is Sliding", PlayerCharacter->GetIsSliding());

	TArray<AActor*> Weapons;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeapon::StaticClass(), Weapons);
	AddFloat(L"Weapon Velocity",
		Weapons[0] == nullptr ? 0.f : Cast<AWeapon>(Weapons[0])->GetWeaponMesh()->GetPhysicsLinearVelocity().Size());
	AddBool(L"Is Character grounded", PlayerCharacter->GetMovementComponent()->IsMovingOnGround());
}

