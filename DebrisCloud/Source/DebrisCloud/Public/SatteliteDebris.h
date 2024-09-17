// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpiceTypes.h"
#include "DebrisCloud/EarthActor.h"
#include "SatteliteDebris.generated.h"

UCLASS()
class DEBRISCLOUD_API ASatteliteDebris : public AActor
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Index = -1; // JSON index

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString TLE1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString TLE2;
	
	UPROPERTY()
	AEarthActor* Earth; //Earth Probably
	
	FSTwoLineElements JSONValue;
	bool StartTickCalculation = false;
	
	
	ASatteliteDebris();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	static FVector LocationFromTLE(const FSEphemerisTime& et, const FSTLEGeophysicalConstants& GeophysicalConstants, const FSTwoLineElements& Elements);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
