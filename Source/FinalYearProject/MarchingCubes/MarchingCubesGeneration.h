// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/Async/AsyncWork.h"

/**
 * 
 */
class FINALYEARPROJECT_API MarchingCubesGeneration : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask <MarchingCubesGeneration>;
public:
	MarchingCubesGeneration(class AChunk* chunk,FVector chunkPos, int vertexCount,
		int zVertexCount, int vertexCountFull, float vertexDistance, float zVertexDistance, int extraVertices, TArray<class NoiseSettings*> noiseSettings);
	
	void CreateMarchingCubesMesh(/*class UProceduralMeshComponent* mesh, UMaterialInterface* material, FVector chunkPos, int vertexCount,
		int zVertexCount, int vertexCountFull, float vertexDistance, float zVertexDistance, int extraVertices*/);

	/*This function is executed when we tell our task to execute*/
	void DoWork();


	float CalculateDesnity(FVector cellPos, FVector ChunkPos, float VertexDistance, float ZVertexDistance, int ExtraVertices);
private:
	class AChunk* chunk;
	FVector chunkPos;
	int vertexCount;
	int zVertexCount;
	int vertexCountFull;
	float vertexDistance;
	float zVertexDistance;
	int extraVertices;
	TArray<class NoiseSettings*> noiseSettings;
	TArray<class FastNoise*> noise;
	/*This function is needed from the API of the engine.
	My guess is that it provides necessary information
	about the thread that we occupy and the progress of our task*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(ChunkMeshCreationThreading, STATGROUP_ThreadPoolAsyncTasks);
	}
};
