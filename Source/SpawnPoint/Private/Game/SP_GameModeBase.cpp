// Copyright Fillipe Romero. All Rights Reserved.


#include "Game/SP_GameModeBase.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ASP_GameModeBase::RequestRespawn(ACharacter* Character, AController* Controller)
{
	if (!IsValid(Character) || !IsValid(Controller)) return;
	
	Character->Reset();
	Character->Destroy();
	
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	ensure(PlayerStarts.Num() > 0);
	const int32 SelectedIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
	
	RestartPlayerAtPlayerStart(Controller, PlayerStarts[SelectedIndex]);
}
