// Fill out your copyright notice in the Description page of Project Settings.

#include "BFL_Core.h"
#include "../EarthActor.h"
#include "Spice.h"
#include "SatteliteDebris.h"
#include "SpiceMath.h"
#include "EngineUtils.h"

#include "DebrisParent.h"
#include "Kismet/GameplayStatics.h"

ASatteliteDebris* UBFL_Core::SpawnCustomSatellite(const FString& OBJECT_ID,const FString& TLE_LINE1, const FString& TLE_LINE2)
{
	UWorld* world = GEngine->GameViewport->GetWorld();
	ES_ResultCode ResultCode;
	FString ErrorMessage;
	FSTwoLineElements Elements;
	FSEphemerisTime et;
	ASatteliteDebris* SatteliteDebris = nullptr;
	if(world)
	{
		TActorIterator<AActor> Iterator(world, AEarthActor::StaticClass());
		for (; Iterator; ++Iterator)
		{
			AEarthActor* Earth = Cast<AEarthActor>(*Iterator);
			FSTLEGeophysicalConstants earth;
			USpice::getgeophs(earth, TEXT("EARTH"));
			USpice::getelm(ResultCode, ErrorMessage, et, Elements, TLE_LINE1, TLE_LINE2);
			if(ResultCode== ES_ResultCode::Success)
				{
					if (Earth != nullptr)
					{
						SatteliteDebris = world->SpawnActor<ASatteliteDebris>(ASatteliteDebris::StaticClass());
						if(SatteliteDebris){
							FTransform t;
							t.SetLocation(LocationFromTLE(et,earth , Elements));
							t.SetScale3D(FVector(5*0.1));
						
							AActor* DebrisActor = UGameplayStatics::GetActorOfClass(world, ADebrisParent::StaticClass());
							SatteliteDebris->SetActorTransform(t);
							ADebrisParent* DebrisParent = Cast<ADebrisParent>(DebrisActor);
							if(DebrisParent)
								SatteliteDebris->AttachToActor(DebrisParent,FAttachmentTransformRules::KeepWorldTransform);
							SatteliteDebris->Index = -1;
							SatteliteDebris->Earth = Earth;
							SatteliteDebris->TLE = Elements;
							SatteliteDebris->JSONValue = nullptr;
							SatteliteDebris->SatelliteData.TLE1 = TLE_LINE1;
							SatteliteDebris->SatelliteData.TLE2 = TLE_LINE2;
							SatteliteDebris->SatelliteData.OBJECTID = OBJECT_ID;
							SatteliteDebris->StartTickCalculation = true;
							SatteliteDebris->DebrisColor = FColor::Green;
						
							//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%d"), i));
							//UE_LOG(LogTemp, Warning, TEXT("%d"),i);
							//InstancedMesh->AddInstance(t, false);
						}
					}
				}
			else
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Can't Calculate TLE, Data is wrong")));

				}
			break;
		}
		
	}

	return SatteliteDebris;
}


FVector UBFL_Core::LocationFromTLE(const FSEphemerisTime& et, const FSTLEGeophysicalConstants& GeophysicalConstants, const FSTwoLineElements& Elements)
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
