// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "LaunchPad.generated.h"

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

	UPROPERTY(VisibleAnywhere, Category=Components)
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, Category=Components)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category=Components)
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	UParticleSystem* LaunchFX;

	UPROPERTY(EditDefaultsOnly, Category="Launch")
	int16 LaunchAngle;

	UPROPERTY(EditDefaultsOnly, Category="Launch")
	int16 Velocity;

	// Annotated with UFUNCTION() so AddDynamic() can use it as a delegate hooked on the OnComponentBegingOverlap() 
	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
