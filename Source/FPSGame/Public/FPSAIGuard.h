// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"

#include "FPSAIGuard.generated.h"

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Enable the guard to sense the player (hear & see)
	UPROPERTY(VisibleAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComponent;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	// FVector is passed by const reference so it's immutable and given 
	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
