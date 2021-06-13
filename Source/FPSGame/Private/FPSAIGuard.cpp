// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"

#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "NavigationSystem.h"
#include "UnrealNetwork.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it
	PrimaryActorTick.bCanEverTick = true;

	// AI related components are handled by the server!
	// No attachment needed, this is not a scene component
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	// AI related methods run on the server!
	// Figuring out what params a delegate function should have:
	//  - go to the delegate method declaration (OnSeenPawn)
	//  - find the DECLARE_DYNAMIC_MULTICAST_DELEGATE where the delegate name param matches the delegate method name
	//  - copy the params except for the delegate name param and the unnecessary commas
	//  - create a delegate method with the necessary params and use it to AddDynamic (add delegate)
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);

	// AI related methods run on the server!
	// Use AddDynamic to handle events with specific delegate methods
	PawnSensingComponent->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	GuardState = EAIState::Idle;

	SwitchPatrolDistance = 250;
}

void AFPSAIGuard::MoveToNextPatrolPoint()
{
	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint)
	{
		CurrentPatrolPoint = FirstPatrolPoint;
	}
	else
	{
		CurrentPatrolPoint = SecondPatrolPoint;
	}

	// Pass in the guard controller and the patrol point it should go to
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr || SeenPawn->IsHidden())
	{
		return;
	}

	// Only valid if called on the server
	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	// Check if exists
	if (GM)
	{
		GM->CompleteMission(SeenPawn, false);
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12,
	                FColor::Green, false, 10.f);

	SetGuardState(EAIState::Alerted);

	AController* GuardController = GetController();
	if (GuardController)
	{
		GuardController->StopMovement();
	}
}

// Location = location of the noise
void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Blue, false, 10.f);

	// Get the direction (X) of the noise based on the difference of the noise Location and guard location
	//  return MakeRotFromX(Target - Start);
	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	// Don't call the FindLookAtRotation blueprint function in Kismet, instead call what it calls by checking its impl
	// Build a rotation matrix from the current rotation of the guard towards the location of the noise
	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	// Make NewLookAt change Yaw only and not Pitch or Roll
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	// Turn the AI character to the noise location
	SetActorRotation(NewLookAt);

	// Reset timer if a new noise event comes in withing the specified TimerHandle timeframe
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);

	// After 3 sec, call ResetOrientation() - stop looking in the noise direction
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	SetGuardState(EAIState::Suspicious);

	// If distracted, stop movement
	AController* GuardController = GetController();
	if (GuardController)
	{
		GuardController->StopMovement();
	}
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::Idle);

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}

// Run on clients
void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChange(GuardState);
}

// Setter created so it can be used in blueprint
void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
	{
		return;
	}

	// Set on the server
	GuardState = NewState;
	// Called on clients
	OnRep_GuardState();

	// Call Blueprint implementable method -> No longer needed -> moved to OnRep_GuardState()
	// OnStateChange(GuardState);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Patrol goal checks
	if (CurrentPatrolPoint)
	{
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();

		// Check if we are within 50 units of our goal, if so - pick a new patrol point
		if (DistanceToGoal < SwitchPatrolDistance)
		{
			MoveToNextPatrolPoint();
		}
	}
}

// Set default rule to replicate variables -> required for props set as Replicated / ReplicatedUsing
void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}
