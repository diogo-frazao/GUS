// Copyright Epic Games, Inc. All Rights Reserved.


#include "GusGameModeBase.h"

#include "Weapon.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

/** Debug Functions */

void AGusGameModeBase::GiveWeapon() const
{
	const ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter == nullptr || WeaponClass == nullptr) { return; }

	const FVector WeaponSpawnPosition = FVector(PlayerCharacter->GetActorLocation().X, PlayerCharacter->GetActorLocation().Y,
	                                            PlayerCharacter->GetActorLocation().Z + 200.f);
	
	GetWorld()->SpawnActor<AWeapon>(WeaponClass, WeaponSpawnPosition, FRotator::ZeroRotator);
}
