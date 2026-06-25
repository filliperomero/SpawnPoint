// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SP_PlayerController.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class SPAWNPOINT_API ASP_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASP_PlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Input")
	TObjectPtr<UInputMappingContext> DefaultIMC;
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Input")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Input")
	TObjectPtr<UInputAction> CrouchAction;
	
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Jump();
	void Input_Crouch();
};
