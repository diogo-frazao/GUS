// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "GusCharacter.h"
#include "Weapon.h"
#include "Components/CapsuleComponent.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::FireWeapon()
{
	if (Character == nullptr || EquippedWeapon == nullptr) { return; }

	if (!EquippedWeapon->CanFire())
	{
		if (EquippedWeapon->CanThrowWeapon())
		{
			ThrowWeapon();
			return;
		}
		return;
	}
	
	EquippedWeapon->WeaponFire(Character->GetFireLocation(), Character->GetFireDirection());
	
	// Play Fire Animation
	UE_LOG(LogTemp, Warning, TEXT("Equipped weapon: %s was fired"), *EquippedWeapon->GetName());
	PlayerCharacterMontage(FireMontage, 1.f);
	Character->PlayFireEffects();
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	// If I already have an equipped weapon, ignore this one
	if (EquippedWeapon != nullptr) { return; }

	EquippedWeapon = WeaponToEquip;
	PlayerCharacterMontage(EquipWeaponMontage, 1.f);
	Character->CallShowCharacterMeshDelayed(0.f, true);

	// Avoid capsule colliding with weapon when being thrown
	Character->GetCapsuleComponent()->IgnoreActorWhenMoving(WeaponToEquip, true);
}

void UCombatComponent::ThrowWeapon()
{
	if (EquippedWeapon == nullptr) { return; }

	UE_LOG(LogTemp, Warning, TEXT("Throwing weapon"));
	
	EquippedWeapon->WeaponThrow(FVector::ZeroVector);

	PlayerCharacterMontage(UnequipWeaponMontage, 1.f);
	
	// 0.2f is enough to let unequipped montage play
	Character->CallShowCharacterMeshDelayed(0.05f, false);

	EquippedWeapon = nullptr;
}

/** Helpers */

void UCombatComponent::PlayerCharacterMontage(UAnimMontage* MontageToPlay, const float& PlayRate) const
{
	UAnimInstance* CharacterAnimInstance = Character->GetCharacterMesh()->GetAnimInstance();
	Character->GetCharacterMesh()->SetHiddenInGame(false, true);
	if (CharacterAnimInstance == nullptr) { return ; }
	CharacterAnimInstance->Montage_Play(MontageToPlay, PlayRate);
}


