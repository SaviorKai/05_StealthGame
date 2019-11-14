// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtractionZone.h"
#include "Components/BoxComponent.h"	// for UBoxComponent
#include "Components/DecalComponent.h"	// for UDecalComponent
#include "FPSCharacter.h" // for AFPSCharacter
#include "FPSGameMode.h" // AFPSGameMode
#include "Kismet/GameplayStatics.h" // UGameplayStatics

// Sets default values
AExtractionZone::AExtractionZone()
{
	OverlapBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBoxComp"));
	OverlapBoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapBoxComp->SetBoxExtent(FVector(200.0f));
	OverlapBoxComp->SetHiddenInGame(false);
	SetRootComponent(OverlapBoxComp);

	OverlapBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AExtractionZone::HandleOverlap);


	//Add Decal Component
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->DecalSize = FVector(200.0f);
	DecalComp->SetupAttachment(OverlapBoxComp);

}



void AExtractionZone::HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) { return; }

	auto OverlappingCharacter = Cast<AFPSCharacter>(OtherActor);							//Casting to AFPSCharacter so that we can access the variables.

	if (OverlappingCharacter->bIsCarryingObjective)											//Check if the bool is true or false
	{
		AFPSGameMode* OurGameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());		//Get game mode and cast to our type.	//Note: GetAuthGameMode() does not work in Multiplayer.(More about this later)
		
		OurGameMode->CompleteMission(OverlappingCharacter, true);
	}
	else
	{
		//PlaySound
		if (!ObjectiveMissingSound) { return; }												//Pointer Protection

		UGameplayStatics::PlaySound2D(this, ObjectiveMissingSound);
	}
}

