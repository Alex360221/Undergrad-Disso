// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lib/NoiseSettings.h"
#include "MapController.generated.h"

UCLASS()
class FINALYEARPROJECT_API AMapController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapController(const FObjectInitializer& ObjectInitializer);
	class APlayerCharacter* playerRef;

	class UMaterialInstanceDynamic* terrainMat;
	UMaterialInterface* masterMat;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	float timer;
	//render settings
	float vertexDistance;
	int vertexCount;
	float chunkSize;
	int playerViewDistance;
	TArray<NoiseSettings*> noiseSettings;
	bool toggleMesh;
	bool toggleMat;

	TMap<FVector2D, class AChunk*> chunks;	//array of chunks

	UFUNCTION(BlueprintCallable)
		void ReGenerateChunks();	//this will regenerate this mesh and it's children
	UFUNCTION(BlueprintCallable)
		void ToggleMeshType();	//this will toggle the mesh type for the chunks
	UFUNCTION(BlueprintCallable)
		void ToggleMat();

	void SpawnChunks(FVector playerPos);	//this will loop through all the chunk poses checking if a chunk should be there
	void SpawnChunk(FVector2D chunkPos);	//this will spawn one chunk
	void RemoveChunks(FVector playerPos);	//this will loop through all the chunks poses cheching if a chunk is outside the area
	void RemoveChunk(FVector2D chunkPos, AChunk* chunk);	//this will remove one chunk
	bool WithinRenderDistance(FVector2D chunkPos, FVector playerPos);	//this will return true if the chunk is within the render distance
public:
	class AChunk* SpawnChunkAtPosWithLOD(FVector2D chunkPos, int LOD, class AChunk* parent, float newVertexDistance);	//this will spawn a chunk at a certain pos with a certian level of detail
	class AChunk* SpawnChunkAtPosWithLOD(FVector chunkPos, int LOD, class AChunk* parent, float newVertexDistance);	//this will spawn a chunk at a certain pos with a certian level of detail
	
	UFUNCTION(BlueprintCallable)
		int NoiseSettings_GetAmountOfLayers() { return noiseSettings.Num() -1 ; }
	UFUNCTION(BlueprintCallable)
		void NoiseSettings_SetOctaves(int octaves, int layer) { if (noiseSettings.IsValidIndex(layer)) { noiseSettings[layer]->octaves = octaves; } }
	UFUNCTION(BlueprintCallable)
		int NoiseSettings_GetOctaves(int layer) { if (noiseSettings.IsValidIndex(layer)) { return noiseSettings[layer]->octaves; } return 3; }
	UFUNCTION(BlueprintCallable)
		void NoiseSettings_SetFrequency(float frequency, int layer) { if (noiseSettings.IsValidIndex(layer)) { noiseSettings[layer]->frequency = frequency; } }
	UFUNCTION(BlueprintCallable)
		float NoiseSettings_GetFrequency(int layer) { if (noiseSettings.IsValidIndex(layer)) { return noiseSettings[layer]->frequency; } return 0.01;}
	UFUNCTION(BlueprintCallable)
		void NoiseSettings_SetStrength(float strength, int layer) { if (noiseSettings.IsValidIndex(layer)) { noiseSettings[layer]->strength = strength; } }
	UFUNCTION(BlueprintCallable)
		float NoiseSettings_GetStrength(int layer) { if (noiseSettings.IsValidIndex(layer)) { return noiseSettings[layer]->strength; } return 1;}
	//UFUNCTION(BlueprintCallable)
		///float MaterialSettings_GetLayer1FallOff() { return terrainMat->GetScalarParameterValue("", ); }
	UFUNCTION(BlueprintCallable)
		UMaterialInstanceDynamic* GetMaterial() { return terrainMat; }
	//	void MaterialSettings_SetLayer1FallOff(float fallOff){terrainMat->setS}

};
