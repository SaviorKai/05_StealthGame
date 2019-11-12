// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtractionZone.h"
#include "Components/BoxComponent.h"

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
}



void AExtractionZone::HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapped with Extraction"));
}

