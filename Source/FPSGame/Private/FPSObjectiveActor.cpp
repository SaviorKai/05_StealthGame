// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSObjectiveActor.h"
#include "Components/SphereComponent.h" // for USphereComponent
#include "Components/StaticMeshComponent.h" // for UStaticMeshComponent
#include "Kismet/GameplayStatics.h" // for UGameplayStatics

// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);							// 
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);								// Sets all the channels to Ignore (just to brush over them all)
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);						// Changes only Pawn to Overlap.
	SphereComp->SetupAttachment(MeshComp);

	

}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void AFPSObjectiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	SpawnEffects();
}



void AFPSObjectiveActor::SpawnEffects()
{
	if (!PickupFX) { return; }														//Pointer Protection
	
	UGameplayStatics::SpawnEmitterAtLocation(
		this,
		PickupFX,
		GetActorLocation()
	);
}

