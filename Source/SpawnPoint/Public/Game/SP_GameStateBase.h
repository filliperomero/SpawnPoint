// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SP_GameStateBase.generated.h"

UCLASS()
class SPAWNPOINT_API ASP_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	bool HasFirstBloodBeenHad() const;
	void UpdateLeader();
	
private:
	bool bHasFirstBloodBeenHad { false };
};
