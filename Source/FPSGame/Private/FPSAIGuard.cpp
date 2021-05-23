// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"

#include "DrawDebugHelpers.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it
	PrimaryActorTick.bCanEverTick = true;

	// No attachment needed, this is not a scene component
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	// Figuring out what params a delegate function should have:
	//  - go to the delegate method declaration (OnSeenPawn)
	//  - find the DECLARE_DYNAMIC_MULTICAST_DELEGATE where the delegate name param matches the delegate method name
	//  - copy the params except for the delegate name param and the unnecessary commas
	//  - create a delegate method with the necessary params and use it to AddDynamic (add delegate)
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);

	// Use AddDynamic to handle events with specific delegate methods
	PawnSensingComponent->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12,
	                FColor::Green, false, 10.f);
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Blue, false, 10.f);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
