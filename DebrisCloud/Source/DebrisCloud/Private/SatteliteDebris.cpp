// Fill out your copyright notice in the Description page of Project Settings.

#include "SatteliteDebris.h"
#include "Spice.h"
#include "SpiceMath.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ASatteliteDebris::ASatteliteDebris()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetMobility(EComponentMobility::Movable);
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Assets/Sphere1.Sphere1'"));
	if(MeshAsset.Object)
	{
		GetStaticMeshComponent()->SetStaticMesh(MeshAsset.Object);
		SphereMesh = MeshAsset.Object;
	}
	OGsatellite = CreateDefaultSubobject<UStaticMeshComponent>("satelliteMesh");
	OGsatellite->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SatMeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/satelites/satelite_meshes/satelite3.satelite3'"));
	if(SatMeshAsset.Object)
	{
		OGsatellite->SetStaticMesh(SatMeshAsset.Object);
		OGsatellite->SetRelativeScale3D(FVector(0.1));
		OGsatellite->SetVisibility(false);
	}
	DebrisColor = FColor::Blue;
	GetStaticMeshComponent()->CastShadow = false;
	GetStaticMeshComponent()->BodyInstance.bNotifyRigidBodyCollision = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	Camera= CreateDefaultSubobject<UCameraComponent>("camera");
	Camera->SetupAttachment(SpringArm);
	
}

// Called when the game starts or when spawned
void ASatteliteDebris::BeginPlay()
{
	Super::BeginPlay();
	UMaterialInstanceDynamic* MaterialInstance = GetStaticMeshComponent()->CreateDynamicMaterialInstance(0, DebrisMaterial);
	MaterialInstance->SetVectorParameterValue("DebrisColor", DebrisColor);
	
	FTimerHandle MyTimerHandle;
	GetWorldTimerManager().SetTimer(
		MyTimerHandle,               
		[this]()                     
		{
			// Code to execute when the timer ends
			UE_LOG(LogTemp, Warning, TEXT("Timer executed!"));
			if(JSONValue)
			{
			SatelliteData.TLE1 = JSONValue->GetStringField(TEXT("TLE_LINE1"));
			SatelliteData.TLE2 = JSONValue->GetStringField(TEXT("TLE_LINE2"));
			SatelliteData.ECCENTRICITY = JSONValue->GetStringField(TEXT("ECCENTRICITY"));
			SatelliteData.INCLINATION = JSONValue->GetStringField(TEXT("INCLINATION"));
			SatelliteData.SEMIMAJOR_AXIS = JSONValue->GetStringField(TEXT("SEMIMAJOR_AXIS"));
				SatelliteData.OBJECTID = JSONValue->GetStringField(TEXT("OBJECT_ID"));
			}
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
		SetActorRelativeLocation(LocationFromTLE(Earth->et, earth, TLE));
	}
}

void ASatteliteDebris::ChangeMesh(bool Sphere)
{
	GetStaticMeshComponent()->SetStaticMesh(Sphere?SphereMesh:SatelliteMesh);
	if(Sphere)
	{
		UMaterialInstanceDynamic* MaterialInstance = GetStaticMeshComponent()->CreateDynamicMaterialInstance(0, DebrisMaterial);
		MaterialInstance->SetVectorParameterValue("DebrisColor", DebrisColor);
		OGsatellite->SetVisibility(false);
		GetStaticMeshComponent()->SetVisibility(true);
	}
	else
	{
		OGsatellite->SetVisibility(true);
		GetStaticMeshComponent()->SetVisibility(false);
	}
}

void ASatteliteDebris::OnCameraYawAxis(float input)
{
	SpringArm->AddLocalRotation(FRotator(0,input,0));
}

void ASatteliteDebris::OnCameraPitchAxis(float input)
{
	SpringArm->AddLocalRotation(FRotator(input,0,0));
}

void ASatteliteDebris::OnCameraZoomInput(float input)
{
	SpringArm->TargetArmLength += input;
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

