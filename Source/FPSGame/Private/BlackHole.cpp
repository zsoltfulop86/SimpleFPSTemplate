// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHole.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ABlackHole::ABlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
	RootComponent = MeshComp;

	// Add collision component to the RootComponent
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereComp->SetupAttachment(MeshComp);
	SphereComp->InitSphereRadius(170);

	// Add radial force to the RootComponent
	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForce->SetupAttachment(MeshComp);
	RadialForce->Radius=3000;
	RadialForce->ForceStrength=-3000000;

	
}

// Called when the game starts or when spawned
void ABlackHole::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlackHole::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	OtherActor->Destroy();

	UGameplayStatics::SpawnEmitterAtLocation(this, BlackHoleFX, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, BlackHoleSound, GetActorLocation());
}

