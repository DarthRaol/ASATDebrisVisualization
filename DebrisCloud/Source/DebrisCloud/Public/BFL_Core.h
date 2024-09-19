// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpiceTypes.h"
#include "BFL_Core.generated.h"

/**
 * 
*/
UCLASS()
class DEBRISCLOUD_API UBFL_Core : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Spawns a custom satellite in the world using TLE data
	UFUNCTION(BlueprintCallable, Category = "Satellite")
	static ASatteliteDebris* SpawnCustomSatellite(const FString& OBJECT_ID,const FString& TLE_LINE1, const FString& TLE_LINE2);

private:
	// Calculates the location of the satellite using TLE and Ephemeris time
	static FVector LocationFromTLE(const FSEphemerisTime& et, const FSTLEGeophysicalConstants& GeophysicalConstants, const FSTwoLineElements& Elements);
	//static FVector LocationFromTLE(const FSEphemerisTime& et, const FSTLEGeophysicalConstants& GeophysicalConstants, const FSTwoLineElements& Elements);

};