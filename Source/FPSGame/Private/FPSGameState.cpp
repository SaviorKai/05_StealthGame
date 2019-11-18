// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "Engine/World.h"
#include "FPSPlayerController.h" // AFPSPlayerController
#include "FPSCharacter.h" // AFPSCharacter



/// [NETWORKING] ///
void AFPSGameState::MultiCastOnMissionCompleted_Implementation(APawn* InstigatorPawn, bool bIsSucessful)
{														
	//// OLD CODE ////
	/*
	/// Loop over all the pawns in the game (this probably includes the enemies)
	for (auto item = GetWorld()->GetPawnIterator(); item; item++)									/// TODO: This Method (GetPawnIterator()) will be deprecated in the future. change the method.
	{
		APawn* Pawn = item->Get();																	// Note: We use "Get()" on these iterators, because the "GetPawnIterator()" returns "weak pointers". Thus, we use "Get()", to get the object, AKA 'Dereference the weak pointer'
		if (!Pawn) { continue; }																	// Pointer Protection
		if (Pawn->IsLocallyControlled())															// Check if this is controlled by a player
		{
			Pawn->DisableInput(nullptr);															
		}
	}
	*/

	for (auto item = GetWorld()->GetPlayerControllerIterator(); item; item++)
	{
		AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(item->Get());			// Note: We use "Get()" on these iterators, because the "GetPawnIterator()" returns "weak pointers". Thus, we use "Get()", to get the object, AKA 'Dereference the weak pointer'
		
		if (!PlayerController) { continue; }														// Pointer Protection

		if (PlayerController->IsLocalController())													// Check if the controller is owned by the current "local" client/server. Thus, it won't run again on the server.
		{
			// Execute the event on the controller (Blueprint Implementable event)
			PlayerController->OnMissionCompleted(InstigatorPawn, bIsSucessful);						// Call the function on the player controller to create the UI Widget.
			
			// Disable input for it's pawn
			if (!PlayerController->GetPawn()) { return; }											// Pointer Protection
			PlayerController->GetPawn()->DisableInput(nullptr);										/// This turns off the input for the Pawn passed (This doesn't destroy the controller).

			// Turn off Objective Text
			AFPSCharacter* ThePawn = Cast<AFPSCharacter>(PlayerController->GetPawn());
			ThePawn->bIsGameOver = true;
		}

	}
}