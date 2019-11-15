// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h" //APlayerController
#include "Kismet/GameplayStatics.h" // UGameplayStatics
#include "FPSGameState.h" // AFPSGameState

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();

	// Set our Default setting for Game State (This could also be set in the editor of our game mode). 
	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if (!InstigatorPawn) { return; }																// Pointer Protection
	//InstigatorPawn->DisableInput(nullptr);				//[MOVED THIS TO GAME STATE CODE] 						

	//Set Endgame Spectator View
	if (!SpectatingViewportClass) { return; }														// PointerProtection
	auto NewViewTarget = UGameplayStatics::GetActorOfClass(this, SpectatingViewportClass);			// Note: To get the class, we added a variable we can set in blueprints, to pass the correct value.
	
	auto InstigatorPawnController = Cast<APlayerController>(InstigatorPawn->GetController());       // Get Controller, which returns AController, and cast it down to APlayerController

	InstigatorPawnController->SetViewTargetWithBlend(
		NewViewTarget,
		0.5f,
		EViewTargetBlendFunction::VTBlend_Cubic
	);

	/// [NETWORKING] Leverage GameState to run the end game code on all clients (Including server)
	AFPSGameState* GS = GetGameState<AFPSGameState>();												// This gets the game state from the game mode.									
	if (!GS) { return; }																			// Pointer Protection
	
	// Disable Input of all Controlled Pawns
	GS->MultiCastOnMissionCompleted(InstigatorPawn, bMissionSuccess);								// Run the function on the game state (this is the UFUNCTION(NetMulticast, Reliable))
	
	
	OnMissionCompleted(InstigatorPawn, bMissionSuccess);											// This Function is BlueprintImplementable (Thus not defined in C++)
}
