
#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "RestartLevelTrigger.generated.h"

UCLASS()
class GUS_API ARestartLevelTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ARestartLevelTrigger();
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RestartCollision = nullptr;

	/** Method called when player overlapped with restart collision */
	UFUNCTION()
	virtual void OnRestartCollisionOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
