// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSExtractionZone.h"
#include "FPSCharacter.h"
#include "FPSGameMode.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSExtractionZone::AFPSExtractionZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComponent"));
	OverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComponent->SetBoxExtent(FVector(200.0f));
	OverlapComponent->SetHiddenInGame(false);
	// Make this the root component
	RootComponent = OverlapComponent;

	// Add function (delegate) to handle event
	OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &AFPSExtractionZone::HandleOverlap);

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(OverlapComponent);
	DecalComponent->DecalSize = FVector(200);
}

void AFPSExtractionZone::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapped with extraction zone!"));

	AFPSCharacter* MyPawn = Cast<AFPSCharacter>(OtherActor);
	// Return if cast fails
	if (MyPawn == nullptr)
	{
		return;
	}

	if (MyPawn->bIsCarryingObjective)
	{
		// Only valid if called on the server
		AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
		// Check if exists
		if (GM)
		{
			GM->CompleteMission(MyPawn, true);
		}
	}
	else
	{
		// 2D sounds are for UI sounds or maybe music - no 3D location needed
		// :: calls static methods on a class
		UGameplayStatics::PlaySound2D(this, ObjectiveMissing);
	}
}
