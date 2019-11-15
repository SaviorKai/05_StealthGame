// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "Engine/World.h"



/// [NETWORKING] ///
void AFPSGameState::MultiCastOnMissionCompleted_Implementation(APawn* InstigatorPawn, bool bIsSucessful)
{														
	/// Loop over all the pawns in the game (this probably includes the enemies)
	for (auto item = GetWorld()->GetPawnIterator(); item; item++)									//TODO: This Method (GetPawnIterator()) will be deprecated in the future. change the method.
	{
		auto Pawn = item->Get();
		if (!Pawn) { return; }																		//Pointer Protection
		if (Pawn->IsLocallyControlled())															//Check if this is controlled by a player
		{
			Pawn->DisableInput(nullptr);															////This turns off the input for the Pawn passed (This doesn't destroy the controller).
		}
	}

}