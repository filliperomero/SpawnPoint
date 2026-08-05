// Copyright Fillipe Romero. All Rights Reserved.

#include "Player/SP_PlayerState.h"

ASP_PlayerState::ASP_PlayerState()
{
	NetUpdateFrequency = 100.f;
}

void ASP_PlayerState::AddScoredElim()
{
	++ScoredElims;
}

void ASP_PlayerState::AddDefeat()
{
	++Defeats;
}

void ASP_PlayerState::AddHit()
{
	++Hits;
}

void ASP_PlayerState::AddMiss()
{
	++Misses;
}

void ASP_PlayerState::AddHeadShotElim()
{
	++HeadShotElims;
}

void ASP_PlayerState::AddSequentialElim(int32 SequenceCount)
{
	if (SequentialElims.Contains(SequenceCount))
	{
		SequentialElims[SequenceCount]++;
	}
	else
	{
		SequentialElims.Add(SequenceCount, 1);
	}
	
	/**
	 * Reduce the count for all lower sequence counts. This is because a triple elim means a double was scored first.
	 * But we want to count this as just a triple, i.e.: elim 1, elim 2, elim 3 = just a triple, not a double and a triple
	 */
	for (auto& Element : SequentialElims)
	{
		if (Element.Key < SequenceCount && Element.Value > 0)
		{
			Element.Value--;
		}
	}
}

void ASP_PlayerState::UpdateHighestStreak(int32 StreakCount)
{
	if (StreakCount > HighestStreak)
	{
		HighestStreak = StreakCount;
	}
}

void ASP_PlayerState::AddRevengeElim()
{
	++RevengeElims;
}

void ASP_PlayerState::AddDethroneElim()
{
	++DethroneElims;
}

void ASP_PlayerState::AddShowStopperElim()
{
	++ShowStopperElims;
}

void ASP_PlayerState::GotFirstBlood()
{
	bFirstBlood = true;
}

void ASP_PlayerState::IsNowWinner()
{
	bWinner = true;
}
