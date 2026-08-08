// Copyright Fillipe Romero. All Rights Reserved.


#include "Game/SP_GameStateBase.h"

bool ASP_GameStateBase::HasFirstBloodBeenHad() const
{
	return bHasFirstBloodBeenHad;
}

void ASP_GameStateBase::UpdateLeader()
{
	bHasFirstBloodBeenHad = true;
}
