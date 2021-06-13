// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"

#include "FPSAIGuard.generated.h"

UENUM()
enum EAIState
{
	Idle,
	Suspicious,
	Alerted
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Editable per instance
	UPROPERTY(EditInstanceOnly, Category="AI")
	bool bPatrol;

	UPROPERTY(EditInstanceOnly, Category="AI", meta=(EditCondition="bPatrol	"))
	float SwitchPatrolDistance;

	// Can use arrays instead of individual points
	// meta disables editing if bPatrol is false
	UPROPERTY(EditInstanceOnly, Category="AI", meta=(EditCondition="bPatrol"))
	AActor* FirstPatrolPoint;

	UPROPERTY(EditInstanceOnly, Category="AI", meta=(EditCondition="bPatrol"))
	AActor* SecondPatrolPoint;

	// Is set to either First- or SecondPatrolPoint
	AActor* CurrentPatrolPoint;

	void MoveToNextPatrolPoint();

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

	// No exposure needed to Blueprint
	FRotator OriginalRotation;

	UFUNCTION()
	void ResetOrientation();

	FTimerHandle TimerHandle_ResetOrientation;

	// Called only on the client when EAIState changes
	UFUNCTION()
	void OnRep_GuardState();

	// GetLifetimeReplicatedProps is required for it to compile
	// Set the game to react to changes to this property by defining an OnRep function -> sync between server & clients
	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	TEnumAsByte<EAIState> GuardState;

	void SetGuardState(EAIState NewState);

	// Expose to Blueprint
	UFUNCTION(BlueprintImplementableEvent, Category="AI")
	void OnStateChange(EAIState NewState);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
