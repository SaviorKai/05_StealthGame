// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaunchPad.generated.h"

class UBoxComponent;
class UDecalComponent;

UCLASS()
class FPSGAME_API ALaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaunchPad();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UBoxComponent* OverlapBoxComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UStaticMeshComponent* LaunchPadMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UDecalComponent* DecalComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	/// Functions
	// Remember to always include UFUNCTION of binding "OnComponentHit / Overlap" functions.
	UFUNCTION()
		void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void SpawnParticleEffect();

	///Member Vars
	UPROPERTY(EditDefaultsOnly, Category = "LaunchPad")
		UParticleSystem* ParticleEffectSystem;

	UPROPERTY(EditInstanceOnly, Category = "LaunchPad")						//NOTE : EditInstanceOnly
		float LaunchStrength = 1500.0f;
	
	UPROPERTY(EditInstanceOnly, Category = "LaunchPad")						//NOTE : EditInstanceOnly
		float LaunchPitchAngle = 45.0f;
	
};
