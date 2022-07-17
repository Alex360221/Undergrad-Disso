// Fill out your copyright notice in the Description page of Project Settings.


#include "MarchingCubesGeneration.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "Runtime/Core/Public/Async/ParallelFor.h"
#include <FinalYearProject/Lib/FastNoise.h>
#include <FinalYearProject/Lib/Triangulation.h>
#include <FinalYearProject/Chunk.h>
#include <FinalYearProject/Lib/NoiseSettings.h>

MarchingCubesGeneration::MarchingCubesGeneration(class AChunk* chunk, FVector chunkPos, int vertexCount,
	int zVertexCount, int vertexCountFull, float vertexDistance, float zVertexDistance, int extraVertices, TArray<class NoiseSettings*> noiseSettings)
{
	this->chunk = chunk;
	this->chunkPos = chunkPos;
	this->vertexCount = vertexCount;
	this->zVertexCount = zVertexCount;
	this->vertexCountFull = vertexCountFull;
	this->vertexDistance = vertexDistance;
	this->zVertexDistance = zVertexDistance;
	this->extraVertices = extraVertices;
	this->noiseSettings = noiseSettings;
}


void MarchingCubesGeneration::CreateMarchingCubesMesh(/*class UProceduralMeshComponent* mesh, UMaterialInterface* material, FVector chunkPos, int vertexCount
	, int zVertexCount,int vertexCountFull, float vertexDistance, float zVertexDistance, int extraVertices*/)
{
	TMap<FVector, float> cells;
	TArray<FVector4> cellData;
	TArray<FVector> vertices;
	TArray<int> triangles;
	TArray<FVector2D> UVs;
	TArray<FVector> vertices_Full;
	TArray<int> triangles_Full;
	TArray<FVector2D> UVs_Full;
	TArray<int> normalsIndices;
	/*FastNoise fn;
	fn.SetNoiseType(FastNoise::Perlin);
	fn.SetSeed(74874);*/
	//FVector chunkPos = GetActorLocation();
	int halfVertexCount = (vertexCount + 2) / 2;
	int extravertices = 2;
	//stage one is to create all the cell data
	int zCount = zVertexCount + (extraVertices * 2) /*- 1*/;
	int xyCount = (halfVertexCount * 2) + (extraVertices * 2)/* - 1*/;
	int xyzCount = (xyCount * xyCount) * zCount;


	//intilise all the noise settings
	for (int i = 0; i < noiseSettings.Num();i++)
	{
		FastNoise* fn = new FastNoise();
		fn->SetNoiseType(FastNoise::Perlin);
		fn->SetFractalOctaves(noiseSettings[i]->octaves);
		fn->SetFrequency(noiseSettings[i]->frequency);
		fn->SetSeed(74874);
		noise.Add(fn);
	}

	//create cell data
	FCriticalSection mutex;
	ParallelFor(xyzCount, [&](int32 index)
	{
		//work out the z x y pos
		int z = index / (xyCount * xyCount);
		int zl = index - (z * (xyCount * xyCount));
		int x = zl / xyCount;
		int y = zl - (x * xyCount);

		//minus vaules from vaule to get the negtive numbers
		z -= extravertices;
		x -= halfVertexCount + extravertices;
		y -= halfVertexCount + extravertices;

		FVector cellPos = FVector(x * vertexDistance, y * vertexDistance, z * zVertexDistance);
		FVector cell = cellPos;
		//calcualte and add cell desnity 
		mutex.Lock();
		cells.Add(cell, CalculateDesnity(cellPos, chunkPos, vertexDistance, zVertexDistance, extraVertices));
		mutex.Unlock();
	});

	for (int z = extravertices * -1; z < zVertexCount + extravertices; z++)
	{
		for (int x = (halfVertexCount + extravertices) * -1; x < (halfVertexCount + extravertices); x++)
		{
			for (int y = (halfVertexCount + extravertices) * -1; y < (halfVertexCount + extravertices); y++)
			{
				bool addedToMain = false;
				if (x > halfVertexCount * -1 && x <= halfVertexCount - 1 && y > halfVertexCount * -1 && y <= halfVertexCount - 1
					&& z > -1 && z <= zVertexCount)
				{
					addedToMain = true;
					Triangulation::CalculateMarchingCubesTriangulation(FVector(x * vertexDistance, y * vertexDistance, z * zVertexDistance), vertexDistance
						, zVertexDistance, vertexCount, &cells, &vertices, &triangles, &UVs, false, &normalsIndices);
				}
				if (x < (halfVertexCount + extravertices) - 1 && y < (halfVertexCount + extravertices) - 1
					&& z < zVertexCount)
				{
					Triangulation::CalculateMarchingCubesTriangulation(FVector(x * vertexDistance, y * vertexDistance, z * zVertexDistance), vertexDistance
						, zVertexDistance,vertexCount + 4, &cells, &vertices_Full, &triangles_Full, &UVs_Full, addedToMain, &normalsIndices);
				}
			}
		}
	}
	TArray<FVector> normals_Full;
	TArray<FProcMeshTangent> Tangents_Full;
	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;
	TArray <FColor> VertexColour;
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertices_Full, triangles_Full, UVs_Full, normals_Full, Tangents_Full);
	for (int i = 0; i < normalsIndices.Num();i++)
	{
		normals.Add(normals_Full[normalsIndices[i]]);
		tangents.Add(Tangents_Full[normalsIndices[i]]);
	}

	//mesh->CreateMeshSection(0, vertices, triangles, normals, UVs, VertexColour, tangents, true);
	//terrainMesh->CreateMeshSection(0, vertices_Full, triangles_Full, normals_Full, UVs_Full, VertexColour, Tangents_Full, true);
	//mesh->SetMaterial(0, material);
	chunk->vertices = vertices;
	chunk->triangles = triangles;
	chunk->normals = normals;
	chunk->UVs = UVs;
	chunk->Tangents = tangents;
	chunk->generatedMeshVaules = true;

}

void MarchingCubesGeneration::DoWork()
{
	CreateMarchingCubesMesh();
}

float MarchingCubesGeneration::CalculateDesnity(FVector cellPos, FVector ChunkPos, float VertexDistance, float ZVertexDistance, int ExtraVertices)
{
	//work out density
	float rf = 1.0f;
	//float surface = fn.GetNoise(((cellPos.X + chunkPos.X) * 0.01), ((cellPos.Y + chunkPos.Y) * 0.01))/**10.0f*//* * cellPos.Z*/;
	//float surface = fn.GetNoise(((cellPos.X + chunkPos.X) * 0.1), ((cellPos.Y + chunkPos.Y) * 0.1)) * 250.f;
	float data = 0;
	for (int i = 0; i < noise.Num(); i++)
	{
		//CHANGE BETWEEN 2D AND 3D NOISE HERE
		//data += (noise[i]->GetNoise(((cellPos.X + ChunkPos.X) * 0.1), ((cellPos.Y + ChunkPos.Y) * 0.1)) * 250) * noiseSettings[i]->strength;
		data += (noise[i]->GetNoise(((cellPos.X + ChunkPos.X) * 0.1), ((cellPos.Y + ChunkPos.Y) * 0.1),cellPos.Z) * 250) * noiseSettings[i]->strength;

	}
	//cellPos.Z = data;
	//surface *= 4000.0f;
	float surface = data;
	float cellHeight = cellPos.Z + ChunkPos.Z + (ExtraVertices * ZVertexDistance);
	if (cellHeight != 0.0f)
	{
		surface += 3500.f;
		rf = surface / cellHeight;
		if (rf < 0) { rf *= -1; }
	}
	return rf;
}
