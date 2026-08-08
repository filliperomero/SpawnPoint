// Copyright Fillipe Romero. All Rights Reserved.


#include "Game/SP_GameStateBase.h"

#include "Player/SP_PlayerState.h"

bool ASP_GameStateBase::HasFirstBloodBeenHad() const
{
	return bHasFirstBloodBeenHad;
}

void ASP_GameStateBase::UpdateLeaders()
{
	TArray<APlayerState*> LocalSortedPlayers = PlayerArray;
	LocalSortedPlayers.Sort([](const APlayerState& A, const APlayerState& B)
	{
		const ASP_PlayerState* PlayerA = Cast<ASP_PlayerState>(&A);
		const ASP_PlayerState* PlayerB = Cast<ASP_PlayerState>(&B);
		
		return PlayerA->GetScoredElims() > PlayerB->GetScoredElims();
	});
	
	Leaders.Empty();
	
	if (LocalSortedPlayers.Num() > 0)
	{
		int32 HighestScore = 0;
		
		for (APlayerState* Player : LocalSortedPlayers)
		{
			if (ASP_PlayerState* PS = Cast<ASP_PlayerState>(Player); IsValid(PS))
			{
				int32 PlayerScore = PS->GetScoredElims();
			
				if (Leaders.Num() == 0)
				{
					HighestScore = PlayerScore;
					Leaders.Add(PS);
				}
				else if (PlayerScore == HighestScore)
				{
					Leaders.Add(PS);
				}
				else
				{
					break; // Since it is sorted, no need to check further
				}
			}
		}
	}
	
	bHasFirstBloodBeenHad = true;
}

ASP_PlayerState* ASP_GameStateBase::GetSoleLeader() const
{
	if (Leaders.Num() == 1)
	{
		return Leaders[0];
	}
	
	return nullptr;
}

bool ASP_GameStateBase::IsTiedForTheLead(ASP_PlayerState* PlayerState) const
{
	return Leaders.Contains(PlayerState);
}
