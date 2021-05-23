// Fill out your copyright notice in the Description page of Project Settings.


#include "LaunchPad.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALaunchPad::ALaunchPad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetBoxExtent(FVector(75, 75, 50));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(BoxComponent);;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(MeshComponent);

	// Set up delegate method HandleOverlap() on overlap -> using & for function pointer definition (cannot pass const)
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ALaunchPad::HandleOverlap);

	LaunchAngle = 40;
	Velocity = 1200;
}

// Called when the game starts or when spawned
void ALaunchPad::BeginPlay()
{
	Super::BeginPlay();
}

void ALaunchPad::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                               const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapped launch pad"));
	
	FRotator ComponentRotation = GetActorRotation();
	ComponentRotation.Pitch += LaunchAngle;
	FVector LaunchVelocity = ComponentRotation.Vector() * Velocity;

	// If other actor is a character
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		Character->LaunchCharacter(LaunchVelocity, true, true);

		UGameplayStatics::SpawnEmitterAtLocation(this, LaunchFX, MeshComponent->GetComponentLocation());
	}
		// If primitive component is overlapping with physics and overlap events enabled
	else if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulse(LaunchVelocity, NAME_None, true);

		UGameplayStatics::SpawnEmitterAtLocation(this, LaunchFX, MeshComponent->GetComponentLocation());
	}
}

// Called every frame
void ALaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
