// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SP_PlayerState.generated.h"

UCLASS()
class SPAWNPOINT_API ASP_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ASP_PlayerState();

	void AddScoredElim();
	void AddDefeat();
	void AddHit();
	void AddMiss();
	void AddHeadShotElim();
	void AddSequentialElim(int32 SequenceCount);
	void UpdateHighestStreak(int32 StreakCount);
	void AddRevengeElim();
	void AddDethroneElim();
	void AddShowStopperElim();
	void GotFirstBlood();
	void IsNowWinner();
	void SetOnStreak(bool bIsOnStreak);
	bool IsOnStreak() const;
	APlayerState* GetLastAttacker() const;
	void SetLastAttacker(APlayerState* InLastAttacker);
	int32 GetScoredElims() const;
	
private:
	int32 ScoredElims { 0 };
	int32 Defeats { 0 };
	int32 Hits { 0 };
	int32 Misses { 0 };
	bool bOnStreak { false }; /* How many elims do we have since we've spawned? */
	int32 HeadShotElims { 0 };
	TMap<int32, int32> SequentialElims; /* Sequential elims - multiple elims within a short period of time */
	int32 HighestStreak { 0 };
	int32 RevengeElims { 0 };
	int32 DethroneElims { 0 };
	int32 ShowStopperElims { 0 };
	bool bFirstBlood { false };
	bool bWinner { false };
	
	UPROPERTY()
	TWeakObjectPtr<APlayerState> LastAttacker;
};
