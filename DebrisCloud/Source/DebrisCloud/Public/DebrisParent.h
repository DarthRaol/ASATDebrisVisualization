#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpModule.h"
#include "SpiceTypes.h"
#include "DebrisParent.generated.h"

#define DEFAULT_SPACETRACK_USER TEXT("user")
#define DEFAULT_SPACETRACK_PASSWORD TEXT("password")
#define LIVE_URL_BASE TEXT("https://www.space-track.org")
#define TEST_URL TEXT("https://gamedevtricks.com/post/call-satcat-rest-api-from-ue-http/cosmos-1408-remaining.json")
#define TEST_URL2 TEXT("https://gamedevtricks.com/post/call-satcat-rest-api-from-ue-http/fengyun-1c-remaining.json")

// Be sure to read the space-track.org CAREFULLY before accessing the server.
// https://space-track.org/documentation#user_agree
// Do NOT Spam the server with repeated requests!!
// If you're building a client that makes use of data in some way, do NOT allow the
// client to directly connect.  It's against the user agreement.   You would instead
// need to read and cache data from your own server, and connect your client to it.

class UInstancedStaticMeshComponent;
class AEarthActor;



UCLASS()
class DEBRISCLOUD_API ADebrisParent : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ADebrisParent();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInstancedStaticMeshComponent> InstancedMesh;

    FSEphemerisTime CurrentEphemerisTime;

    
    // 1982-092A = COSMOS 1408 DEB
    // 1999-025A = FENGYUN 1C DEB
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString ObjectId = TEXT("1999-025A");

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool UseTestUrl = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString TestUrl = TEST_URL;

    // Be sure to set your credentials appropriately!
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString SpaceTrackUser = DEFAULT_SPACETRACK_USER;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString SpaceTrackPassword = DEFAULT_SPACETRACK_PASSWORD;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float ObjectScale = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UMaterialInstance> DebrisMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FColor DebrisColor;

    UPROPERTY()
    TWeakObjectPtr<AEarthActor> EarthTheActor;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    void LoginWithSpaceTrack(const FString& NewObjectId, const FString& NewUser, const FString& NewPassword);
    void ProcessSpaceTrackResponse(const FString& ResponseContent);
    void ProcessSpaceTrackResponse(const TArray<TSharedPtr<FJsonValue>>& JsonResponseArray);
    void SpaceTrackBeginResponse();
    void SpaceTrackEndResponse(const FSEphemerisTime& et);
    void ProcessSpaceTrackResponseObject(const TSharedPtr<FJsonObject>& JsonResponseObject);
    FVector LocationFromTLE(const FSEphemerisTime& et, const FSTLEGeophysicalConstants& GeophysicalConstants, const FSTwoLineElements& Elements);

private:
    TArray<FSTwoLineElements> DebrisElements;

};
