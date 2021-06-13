// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSObjectiveActor.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set up the mesh that will act as the root component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	// Set up the root component -> provides the transform etc. of an actor
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	// Set up and attach a sphere to the root component
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	// E = enum
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(MeshComp);

	// Sync objective status
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();

	PlayEffects();
}


// Called every frame
void AFPSObjectiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Spawn effect
void AFPSObjectiveActor::PlayEffects()
{
	// spawn and other useful static functions found in `UGameplayStatics`
	UGameplayStatics::SpawnEmitterAtLocation(this, PickupFX, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
}

// Called when overlapping on both client and server -> default for overlap events
void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Always call super when overriding
	Super::NotifyActorBeginOverlap(OtherActor);

	// Will run on client and server
	PlayEffects();

	// Will run as a server, because it's gameplay related
	if (GetLocalRole() == ROLE_Authority)
	{
		AFPSCharacter* MyCharacter = Cast<AFPSCharacter>(OtherActor);

		MyCharacter->bIsCarryingObjective = true;

		Destroy();
	}
}
