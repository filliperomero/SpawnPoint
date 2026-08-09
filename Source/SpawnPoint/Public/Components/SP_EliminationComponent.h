// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SP_EliminationComponent.generated.h"

class ASP_GameStateBase;
enum class ESpecialElimType : uint16;
class ASP_PlayerState;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPAWNPOINT_API USP_EliminationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USP_EliminationComponent();
	
	UFUNCTION()
	void OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal);
	
	UPROPERTY(EditDefaultsOnly, Category = "SpawnPoint|Elimination")
	float SequentialElimInterval { 2.f };
	
	UPROPERTY(EditDefaultsOnly, Category = "SpawnPoint|Elimination")
	int32 ElimsNeededForStreak { 5 };

protected:
	
private:
	float LastElimTime { 0.f };
	int32 SequentialElims { 0 };
	int32 Streak { 0 };
	
	static ASP_PlayerState* GetPlayerStateFromActor(AActor* Actor);
	void ProcessHitOrMiss(bool bHit, ASP_PlayerState* AttackerPS);
	void ProcessElimination(bool bHeadShot, ASP_PlayerState* AttackerPS, ASP_PlayerState* VictimPS);
	void ProcessHeadshot(bool bHeadShot, ESpecialElimType& OutElimType, ASP_PlayerState* AttackerPS);
	void ProcessSequentialEliminations(ESpecialElimType& OutElimType, ASP_PlayerState* AttackerPS);
	void ProcessStreaks(ESpecialElimType& OutElimType, ASP_PlayerState* AttackerPS, ASP_PlayerState* VictimPS);
	void HandleFirstBlood(ASP_GameStateBase* GameState, ESpecialElimType& OutElimType, ASP_PlayerState* AttackerPS);
	void UpdateLeaderStatus(ASP_GameStateBase* GameState, ESpecialElimType& OutElimType, ASP_PlayerState* AttackerPS, ASP_PlayerState* VictimPS);
	bool HasSpecialElimTypes(const ESpecialElimType& SpecialElimType) const;
};
