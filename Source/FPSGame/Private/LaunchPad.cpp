// Fill out your copyright notice in the Description page of Project Settings.


#include "LaunchPad.h"
#include "Components/BoxComponent.h"	// for UBoxComponent
#include "Components/StaticMeshComponent.h" // UStaticMeshComponent
#include "Components/DecalComponent.h"	// for UDecalComponent
#include "FPSCharacter.h" //FPSCharacter
#include "Kismet/GameplayStatics.h" //UGameplayStatics


// Sets default values
ALaunchPad::ALaunchPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Add the root mesh
	LaunchPadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaunchPadMesh"));
	SetRootComponent(LaunchPadMesh);

	//Add the UBoxComponent for overlap event checking
	OverlapBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBoxComp"));
	OverlapBoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);										
	OverlapBoxComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	OverlapBoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapBoxComp->SetupAttachment(LaunchPadMesh);

	//Bind the delegate to function.
	OverlapBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ALaunchPad::HandleOverlap);
	
	//Add Decal Component
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->DecalSize = FVector(200.0f);
	DecalComp->SetupAttachment(LaunchPadMesh);
}

// Called when the game starts or when spawned
void ALaunchPad::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALaunchPad::HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (!OtherActor) { return; } //Pointer Protection

	if (Cast<APawn>(OtherActor))
	{
		///If it is not a Pawn.
		//Calculate the force required
		auto LaunchAngle = FRotator(90, 0, 0);													//TODO: Remove Magic Number(s)
		auto LaunchSpeed = GetRootComponent()->GetForwardVector() + LaunchAngle.Vector();
		auto ApplyForce = LaunchSpeed * 1000;

		//Cast to ACharacter type (Our version), and Launch Character
		Cast<AFPSCharacter>(OtherActor)->LaunchCharacter(										//Launch the character. Triggers the OnLaunched event.
			ApplyForce,																			//TODO: Remove Magic Number(s)
			true,																				//if true, replace the XY velocity of the character, instead of adding to it.
			true																				//if true, replace the z velocity of the character, instead of adding to it.
		);

		//Particle Effect on use
		SpawnParticleEffect();
	}
	else
	{
		///If it's not a Pawn.
		//Cast to the correct type (So we can call Apply force) and Get the root component
		auto OverlappingActor = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent());
		
		//Calculate the force required
		auto LaunchAngle = FRotator(120, 0, 0);													//TODO: Remove Magic Number(s)
		auto LaunchSpeed = GetRootComponent()->GetForwardVector() + LaunchAngle.Vector();
		auto ApplyForce = LaunchSpeed * 250000;													//TODO: Remove Magic Number(s)

		//Add Force
		OverlappingActor->AddForce(
			ApplyForce,
			NAME_None,
			true
		);
		
		//Particle Effect on use
		SpawnParticleEffect();
	}
}

void ALaunchPad::SpawnParticleEffect()
{
	//Particle Effect on use
	if (!ParticleEffectSystem) { return; }													//Pointer Protection
	UGameplayStatics::SpawnEmitterAtLocation(
		this,
		ParticleEffectSystem,
		GetActorLocation()
	);
}

