// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SP_EliminationComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPAWNPOINT_API USP_EliminationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USP_EliminationComponent();
	
	UFUNCTION()
	void OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal);

protected:
	
private:
	
};
