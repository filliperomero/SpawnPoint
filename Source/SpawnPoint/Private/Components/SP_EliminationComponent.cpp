// Copyright Fillipe Romero. All Rights Reserved.

#include "Components/SP_EliminationComponent.h"

#include "Engine/World.h"
#include "Game/SP_GameStateBase.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SP_PlayerState.h"
#include "Types/SP_Types.h"

USP_EliminationComponent::USP_EliminationComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void USP_EliminationComponent::OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal)
{
	ASP_PlayerState* AttackerPS = GetPlayerStateFromActor(Attacker);
	if (!IsValid(AttackerPS)) return;
	
	ProcessHitOrMiss(bHit, AttackerPS);
	
	// Early return if it was a miss
	if (!bHit) return;
	
	ASP_PlayerState* VictimPS = GetPlayerStateFromActor(Victim);
	if (!IsValid(VictimPS)) return;
	
	if (bLethal)
	{
		ProcessElimination(bHeadShot, AttackerPS, VictimPS);
	}
}

void USP_EliminationComponent::ProcessHitOrMiss(bool bHit, ASP_PlayerState* AttackerPS)
{
	if (bHit)
	{
		AttackerPS->AddHit();
	}
	else
	{
		AttackerPS->AddMiss();
	}
}

void USP_EliminationComponent::ProcessElimination(bool bHeadShot, ASP_PlayerState* AttackerPS, ASP_PlayerState* VictimPS)
{
	AttackerPS->AddScoredElim();
	VictimPS->AddDefeat();
	
	ESpecialElimType SpecialElimType{};
	
	ProcessHeadshot(bHeadShot, SpecialElimType, AttackerPS);
	ProcessSequentialEliminations(SpecialElimType, AttackerPS);
	ProcessStreaks(SpecialElimType, AttackerPS, VictimPS);
	
	ASP_GameStateBase* GameState = Cast<ASP_GameStateBase>(UGameplayStatics::GetGameState(AttackerPS));
	
	if (IsValid(GameState))
	{
		HandleFirstBlood(GameState, SpecialElimType, AttackerPS);
		UpdateLeaderStatus(GameState, SpecialElimType, AttackerPS, VictimPS);
	}
	
	if (HasSpecialElimTypes(SpecialElimType))
	{
		AttackerPS->Client_SpecialElim(SpecialElimType, SequentialElims, Streak, AttackerPS->GetScoredElims());
	}
	else
	{
		AttackerPS->Client_ScoredElim(AttackerPS->GetScoredElims());
	}
}

void USP_EliminationComponent::ProcessHeadshot(bool bHeadShot, ESpecialElimType& OutElimType, ASP_PlayerState* AttackerPS)
{
	if (bHeadShot)
	{
		OutElimType |= ESpecialElimType::HeadShot; // 00000000 00000000 | 00000000 00000001 => 00000000 00000001
		AttackerPS->AddHeadShotElim();
	}
}

void USP_EliminationComponent::ProcessSequentialEliminations(ESpecialElimType& OutElimType, ASP_PlayerState* AttackerPS)
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	
	if (CurrentTime - LastElimTime <= SequentialElimInterval)
	{
		++SequentialElims;
	}
	else
	{
		SequentialElims = 1;
	}
	
	LastElimTime = CurrentTime;
	
	if (SequentialElims > 1)
	{
		OutElimType |= ESpecialElimType::Sequential;
		AttackerPS->AddSequentialElim(SequentialElims);
	}
}

void USP_EliminationComponent::ProcessStreaks(ESpecialElimType& OutElimType, ASP_PlayerState* AttackerPS, ASP_PlayerState* VictimPS)
{
	++Streak;
	
	if (Streak >= ElimsNeededForStreak)
	{
		OutElimType |= ESpecialElimType::Streak;
		AttackerPS->SetOnStreak(true);
		AttackerPS->UpdateHighestStreak(Streak);
	}
	
	if (VictimPS->IsOnStreak())
	{
		OutElimType |= ESpecialElimType::Showstopper;
		AttackerPS->AddShowStopperElim();
		VictimPS->SetOnStreak(false);
	}
	
	if (AttackerPS->GetLastAttacker() == VictimPS)
	{
		OutElimType |= ESpecialElimType::Revenge;
		AttackerPS->AddRevengeElim();
		AttackerPS->SetLastAttacker(nullptr);
	}
	
	VictimPS->SetLastAttacker(AttackerPS);
}

void USP_EliminationComponent::HandleFirstBlood(ASP_GameStateBase* GameState, ESpecialElimType& OutElimType, ASP_PlayerState* AttackerPS)
{
	if (!GameState->HasFirstBloodBeenHad())
	{
		OutElimType |= ESpecialElimType::FirstBlood;
		AttackerPS->GotFirstBlood();
	}
}

void USP_EliminationComponent::UpdateLeaderStatus(ASP_GameStateBase* GameState, ESpecialElimType& OutElimType, ASP_PlayerState* AttackerPS, ASP_PlayerState* VictimPS)
{
	ASP_PlayerState* LastLeader = GameState->GetSoleLeader();
	const bool bAttackerWasTiedForTheLead = GameState->IsTiedForTheLead(AttackerPS);
	
	GameState->UpdateLeaders();
	
	// Attacker was not tied for the lead before and is now tied for the lead
	if (!bAttackerWasTiedForTheLead && GameState->IsTiedForTheLead(AttackerPS))
	{
		OutElimType |= ESpecialElimType::TiedTheLeader;
	}
	
	if (IsValid(LastLeader) && LastLeader != GameState->GetSoleLeader())
	{
		// Last leader has lost the lead
		LastLeader->Client_LostTheLead();
		
		if (VictimPS == LastLeader)
		{
			OutElimType |= ESpecialElimType::Dethrone;
			AttackerPS->AddDethroneElim();
		}
	}
	
	if (AttackerPS != LastLeader && AttackerPS == GameState->GetSoleLeader())
	{
		OutElimType |= ESpecialElimType::GainedTheLead;
	}
}

bool USP_EliminationComponent::HasSpecialElimTypes(const ESpecialElimType& SpecialElimType) const
{
	return static_cast<uint16>(SpecialElimType) != 0;
}

ASP_PlayerState* USP_EliminationComponent::GetPlayerStateFromActor(AActor* Actor)
{
	if (APawn* Pawn = Cast<APawn>(Actor); IsValid(Pawn))
	{
		return Pawn->GetPlayerState<ASP_PlayerState>();
	}
	
	return nullptr;
}
