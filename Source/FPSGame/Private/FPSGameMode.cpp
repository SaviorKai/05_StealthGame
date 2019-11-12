// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h" //APlayerController
#include "Kismet/GameplayStatics.h" // UGameplayStatics

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn)
{
	if (!InstigatorPawn) { return; }							//Pointer Protection
	
	InstigatorPawn->DisableInput(nullptr);						//This turns off the input for the Pawn passed (This doesn't destroy the controller).

	OnMissionCompleted(InstigatorPawn);							//This Function is BlueprintImplementable (Thus not defined in C++)

	
	//Set Endgame Spectator View
	if (!SpectatingViewportClass) { return; }														//PointerProtection
	auto NewViewTarget = UGameplayStatics::GetActorOfClass(this, SpectatingViewportClass);			// Note: To get the class, we added a variable we can set in blueprints, to pass the correct value.
	
	auto InstigatorPawnController = Cast<APlayerController>(InstigatorPawn->GetController());       // Get Controller, which returns AController, and cast it down to APlayerController

	InstigatorPawnController->SetViewTargetWithBlend(
		NewViewTarget,
		0.5f,
		EViewTargetBlendFunction::VTBlend_Cubic
	);
}
