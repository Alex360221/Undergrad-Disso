// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/Async/AsyncWork.h"

/**
 * 
 */
class FINALYEARPROJECT_API HeightMapGeneration : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask <HeightMapGeneration>;
public:
	HeightMapGeneration(class AChunk* chunk, class UProceduralMeshComponent* mesh,UMaterialInterface* material, FVector chunkPos, int vertexCount,
		int vertexCountFull, float vertexDistance, int extraVertices, TArray<class NoiseSettings*> noiseSettings);

	/*This function is executed when we tell our task to execute*/
	void DoWork();
	
	void CreateHeightMap(/*class UProceduralMeshComponent* mesh, UMaterialInterface* material, FVector chunkPos, int vertexCount,
		int vertexCountFull, float vertexDistance, int extraVertices*/);
private:
	class AChunk* chunk;
	class UProceduralMeshComponent* mesh;
	UMaterialInterface* material;
	FCriticalSection critical;
	FVector chunkPos;
	int vertexCount;
	int vertexCountFull;
	float vertexDistance;
	int extraVertices;
	TArray<class NoiseSettings*> noiseSettings;
	/*This function is needed from the API of the engine.
	My guess is that it provides necessary information
	about the thread that we occupy and the progress of our task*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(ChunkMeshCreationThreading, STATGROUP_ThreadPoolAsyncTasks);
	}
};
