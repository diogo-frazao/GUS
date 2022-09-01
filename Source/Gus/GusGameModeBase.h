// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GusGameModeBase.generated.h"

UCLASS()
class GUS_API AGusGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void FadeOutAndRestartLevel();

	UFUNCTION(BlueprintImplementableEvent)
	void FadeOutAndGoToNextLevel();

private:
	/** Weapon class to spawn on debug functions */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AWeapon> WeaponClass = nullptr;

	/** Drops a weapon near the player */
	UFUNCTION(Exec)
	void GiveWeapon() const;

public:
	FORCEINLINE void CallFadeOutAndRestartLevel() { FadeOutAndRestartLevel(); }
	FORCEINLINE void CallFadeOutAndGoToNextLevel() { FadeOutAndGoToNextLevel(); }
};
