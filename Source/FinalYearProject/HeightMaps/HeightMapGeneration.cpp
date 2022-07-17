// Fill out your copyright notice in the Description page of Project Settings.


#include "HeightMapGeneration.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include <FinalYearProject/Lib/FastNoise.h>
#include <FinalYearProject/Lib/Triangulation.h>
#include <FinalYearProject/Chunk.h>
#include <FinalYearProject/Lib/NoiseSettings.h>

HeightMapGeneration::HeightMapGeneration(class AChunk* chunk, class UProceduralMeshComponent* mesh, UMaterialInterface* material, FVector chunkPos, int vertexCount,
	int vertexCountFull, float vertexDistance, int extraVertices, TArray<class NoiseSettings*> noiseSettings)
{
	this->chunk = chunk;
	this->mesh = mesh;
	this->material = material;
	this->chunkPos = chunkPos;
	this->vertexCount = vertexCount;
	this->vertexCountFull = vertexCountFull;
	this->vertexDistance = vertexDistance;
	this->extraVertices = extraVertices;
	this->noiseSettings = noiseSettings;
}

void HeightMapGeneration::DoWork()
{
	CreateHeightMap(/*mesh, material, chunkPos, vertexCount, vertexCountFull, vertexDistance, extraVertices*/);
}

void HeightMapGeneration::CreateHeightMap(/*UProceduralMeshComponent* mesh, UMaterialInterface* material, FVector chunkPos, int vertexCount,
	int vertexCountFull, float vertexDistance, int extraVertices*/)
{
	//create all the varibles here as they need to be static
	TArray<FVector> vertices;
	TArray<int> triangles;
	TArray<FVector2D> UVs;

	TArray<FVector> vertices_Full;
	TArray<int> triangles_Full;
	TArray<FVector2D> UVs_Full;
	TArray<int> normalsIndices;	//this stores which normal should be used for the mesh
	//loop through vertices on x and y
	//create vertex data 
	//FVector chunkPos = GetActorLocation();
	float uvScale = 1 / (vertexDistance * (vertexCountFull + (extraVertices * 2)));
	TArray<FastNoise*> noise;
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

	FVector vertexPos = FVector(0, 0, 0);
	int halfVertexCount = (vertexCount / 2);
	int halfVertexCountFull = halfVertexCount + 1;
	for (int x = (halfVertexCountFull + extraVertices) * -1; x <= halfVertexCount/*vertexCount*/ + extraVertices; x++)
	{
		vertexPos.X = x * vertexDistance;
		for (int y = (halfVertexCountFull + extraVertices) * -1; y <= halfVertexCount/*vertexCount*/ + extraVertices; y++)
		{
			vertexPos.Y = y * vertexDistance;
			//get Noise

			//vertexPos.Z =  (fn.GetNoise(((vertexPos.X + chunkPos.X) * 0.1), ((vertexPos.Y + chunkPos.Y) * 0.1)) * 250);
			float data = 0;
			for (int i = 0; i < noise.Num(); i++)
			{
				//CHANGE BETWEEN 2D AND 3D NOISE HERE
				//data += (noise[i]->GetNoise(((vertexPos.X + chunkPos.X) * 0.1), ((vertexPos.Y + chunkPos.Y) * 0.1)) * 250) * noiseSettings[i]->strength;
				data += (noise[i]->GetNoise(((vertexPos.X + chunkPos.X) * 0.1), ((vertexPos.Y + chunkPos.Y) * 0.1), 2709.f) * 250) * noiseSettings[i]->strength;
			}
			vertexPos.Z = data;
			vertexPos.Z += 2709.f;
			int index = 0;
			if (x >= halfVertexCountFull * -1 && x <= halfVertexCount && y >= halfVertexCountFull * -1 && y <= halfVertexCount)
			{//in section which needs to be part of the mesh
				int i = vertices.Add(vertexPos);
				index = vertices_Full.Add(vertexPos);
				normalsIndices.Add(index);
				UVs.Add(FVector2D(vertexPos.X * uvScale, vertexPos.Y * uvScale));
				UVs_Full.Add(FVector2D(vertexPos.X * uvScale, vertexPos.Y * uvScale));
				//do normal triangulation
				if (x < halfVertexCount && y < halfVertexCount)
				{//this is needed as triugnaltion uses two vertices, this stops it going outside the mesh bounds
					Triangulation::CalculateTriangulation(i, vertexCountFull + 1, &triangles);
				}
			}
			else
			{
				index = vertices_Full.Add(vertexPos);
				UVs_Full.Add(FVector2D(vertexPos.X * uvScale, vertexPos.Y * uvScale));
			}
			//do full triangulation
			if (x < halfVertexCount + extraVertices && y < halfVertexCount + extraVertices)
			{
				Triangulation::CalculateTriangulation(index, vertexCountFull + (extraVertices * 2) + 1, &triangles_Full);
			}
		}
	}

	TArray<FVector> normals_Full;
	TArray <FColor> VertexColour;
	TArray<FProcMeshTangent> Tangents_Full;
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertices_Full, triangles_Full, UVs_Full, normals_Full, Tangents_Full);
	//chose whcih normals to use
	TArray<FVector> normals;
	TArray<FProcMeshTangent> Tangents;
	for (int i = 0; i < normalsIndices.Num();i++)
	{
		normals.Add(normals_Full[normalsIndices[i]]);
		Tangents.Add(Tangents_Full[normalsIndices[i]]);
	}

	chunk->vertices = vertices;
	chunk->triangles = triangles;
	chunk->normals = normals;
	chunk->UVs = UVs;
	chunk->Tangents = Tangents;
	chunk->generatedMeshVaules = true;
}

