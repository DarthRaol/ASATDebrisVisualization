// Fill out your copyright notice in the Description page of Project Settings.

#include "Spice.h"
#include "SpiceMath.h"
#include "SatteliteDebris.h"

// Sets default values
ASatteliteDebris::ASatteliteDebris()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComp);
	UStaticMeshComponent* SatteliteMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sattelite"));
	SatteliteMesh->SetupAttachment(RootComp);
	SatteliteMesh->SetHiddenInGame(true);
}

// Called when the game starts or when spawned
void ASatteliteDebris::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle MyTimerHandle;
	GetWorldTimerManager().SetTimer(
		MyTimerHandle,               
		[this]()                     
		{
			// Code to execute when the timer ends
			UE_LOG(LogTemp, Warning, TEXT("Timer executed!"));
			
		},
		5.0f,                        
		false                        
	);
}

// Called every frame
void ASatteliteDebris::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(StartTickCalculation)
	{
		FSTLEGeophysicalConstants earth;
		USpice::getgeophs(earth, TEXT("EARTH"));
		SetActorRelativeLocation(LocationFromTLE(Earth->et, earth, JSONValue));
	}
}

FVector ASatteliteDebris::LocationFromTLE(const FSEphemerisTime& et, const FSTLEGeophysicalConstants& GeophysicalConstants, const FSTwoLineElements& Elements)
{
	ES_ResultCode ResultCode;
	FString ErrorMessage;
	FVector scenegraphPosition = FVector(0.);

	FSStateVector state;
	USpice::evsgp4(ResultCode, ErrorMessage, state, et, GeophysicalConstants, Elements);

	if (ResultCode == ES_ResultCode::Success)
	{
		if (FGenericPlatformMath::IsNaN(state.r.x.AsKilometers())
			|| FGenericPlatformMath::IsNaN(state.r.y.AsKilometers())
			|| FGenericPlatformMath::IsNaN(state.r.z.AsKilometers())
			|| FGenericPlatformMath::IsNaN(state.v.dx.AsKilometersPerSecond())
			|| FGenericPlatformMath::IsNaN(state.v.dy.AsKilometersPerSecond())
			|| FGenericPlatformMath::IsNaN(state.v.dz.AsKilometersPerSecond()))
		{
			UE_LOG(LogTemp, Error, TEXT("evsgp4 returned a NaN"));
		}
		else
		{
			// The state is in Ref=J2000, and so is the scene, so no need to xform
			scenegraphPosition = MaxQ::Math::Swizzle(state.r);
		}
	}

	return scenegraphPosition;
}

