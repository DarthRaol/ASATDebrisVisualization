// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
#include "SpiceTypes.h"
#include "Camera/CameraComponent.h"
#include "DebrisCloud/EarthActor.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/SpringArmComponent.h"
#include "SatteliteDebris.generated.h"



USTRUCT(BlueprintType)
struct FSatelliteData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString OBJECTID;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString TLE1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString TLE2;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString ECCENTRICITY;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString INCLINATION;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString SEMIMAJOR_AXIS;
	

	FSatelliteData()
	{
		OBJECTID = "";
		TLE1 = "";
		TLE2 = "";
		ECCENTRICITY = "";
		INCLINATION = "";
		SEMIMAJOR_AXIS = "";
		
	}

};


UCLASS()
class DEBRISCLOUD_API ASatteliteDebris : public AStaticMeshActor
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Index = -1; // JSON index

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FSatelliteData SatelliteData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DebrisMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FColor DebrisColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> SphereMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> SatelliteMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> OGsatellite;
	
	
	UPROPERTY()
	AEarthActor* Earth; //Earth Probably
	
	TSharedPtr<FJsonObject> JSONValue;
	bool StartTickCalculation = false;

	FSTwoLineElements TLE;
	ASatteliteDebris();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	static FVector LocationFromTLE(const FSEphemerisTime& et, const FSTLEGeophysicalConstants& GeophysicalConstants, const FSTwoLineElements& Elements);

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ChangeMesh(bool Sphere);

	UFUNCTION(BlueprintCallable)
	void OnCameraYawAxis(float input);

	UFUNCTION(BlueprintCallable)
	void OnCameraPitchAxis(float input);

	UFUNCTION(BlueprintCallable)
	void OnCameraZoomInput(float input);
};
