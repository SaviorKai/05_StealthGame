// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UFUNCTION(NetMulticast, Reliable)													//NetMultiCast = This will ensure this is run on all clients (Including the server)
		void MultiCastOnMissionCompleted(APawn* InstigatorPawn, bool bIsSucessful);
	
};
