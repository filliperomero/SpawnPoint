// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SP_GameStateBase.generated.h"

class ASP_PlayerState;

UCLASS()
class SPAWNPOINT_API ASP_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	bool HasFirstBloodBeenHad() const;
	void UpdateLeaders();
	ASP_PlayerState* GetSoleLeader() const;
	bool IsTiedForTheLead(ASP_PlayerState* PlayerState) const;
	
private:
	bool bHasFirstBloodBeenHad { false };
	
	UPROPERTY()
	TArray<TObjectPtr<ASP_PlayerState>> Leaders;
};
