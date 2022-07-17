// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "Lib/FastNoise.h"
#include "KismetProceduralMeshLibrary.h"
#include "PlayerCharacter.h"
#include "MapController.h"
#include "HeightMaps/HeightMapGeneration.h"
#include "MarchingCubes/MarchingCubesGeneration.h"
#include <FinalYearProject/Lib/Triangulation.h>
#include "Lib/NoiseSettings.h"

// Sets default values
AChunk::AChunk(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	vertexDistance = 50;
	vertexCount = 16;
	zVertexCount = vertexCount;
	zVertexDistance = 400;
	bottomTriPoint = 0;
	topTriPoint = 0;
	chunk_LOD = 0;
	SetVaules = false;
	hasDivided = false;
	createdMesh = false;
	marchingCubes = true;
	generatedMeshVaules = false;
	meshesCreated = 0;
	toggleType = true;
	toggleMat = true;

	chunkCreationTime = 0.0f;

	terrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Terrain Mesh"));
	terrainMesh->SetupAttachment(RootComponent);
	SetRootComponent(terrainMesh);
	//Material'/Game/Mats/NormalsMat.NormalsMat'

	normalsMat = CreateDefaultSubobject<UMaterialInterface>(TEXT("Normal Mat"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material2(TEXT("Material'/Game/Mats/NormalsMat.NormalsMat'"));
	normalsMat = Material2.Object;
	////Material'/Game/Mats/TerrainMatMarching.TerrainMatMarching'
	//terrainMatMarching = CreateDefaultSubobject<UMaterialInterface>(TEXT("Terrain Mat Marching"));
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material2(TEXT("Material'/Game/Mats/TerrainMatMarching.TerrainMatMarching'"));
	//terrainMatMarching = Material2.Object;
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AChunk::Update(float DeltaTime)
{
	if (SetVaules)
	{		
		if (generatedMeshVaules && !createdMesh)
		{
			createdMesh = true;
			TArray <FColor> VertexColour;
			terrainMesh->ClearMeshSection(0);
			terrainMesh->CreateMeshSection(0, vertices, triangles, normals, UVs, VertexColour, Tangents, false);
			if(toggleMat){ terrainMesh->SetMaterial(0, terrainMat); }
			else { terrainMesh->SetMaterial(0, normalsMat); }
			vertices.Empty();
			triangles.Empty();
			normals.Empty();
			UVs.Empty();
			Tangents.Empty();
					
		}
		if (hasDivided && terrainMesh->IsMeshSectionVisible(0))
		{
			if (tR->terrainMesh->IsMeshSectionVisible(0) && tL->terrainMesh->IsMeshSectionVisible(0)
				&& bR->terrainMesh->IsMeshSectionVisible(0) && bL->terrainMesh->IsMeshSectionVisible(0))
			{
				terrainMesh->SetVisibility(false);
			}
		}
		UpdateLOD();

		if (hasDivided)
		{
			tR->Update(DeltaTime);
			tL->Update(DeltaTime);
			bR->Update(DeltaTime);
			bL->Update(DeltaTime);
		}
	}
}

void AChunk::Start()
{
	SetVaules = true;
	vertexCountFull = vertexCount + 1;
	CreatePerlinNoiseMesh();
}

void AChunk::CreateNoiseSettings(TArray<class NoiseSettings*> newNoiseSettings)
{
	noiseSettings.Empty();
	for (int i = 0; i < newNoiseSettings.Num();i++)
	{
		this->noiseSettings.Add(new NoiseSettings(newNoiseSettings[i]->octaves, newNoiseSettings[i]->frequency, newNoiseSettings[i]->strength));
	}
}

void AChunk::ReGenerateChunk()
{
	generatedMeshVaules = false;
	createdMesh = false;
	CreatePerlinNoiseMesh();
	if (hasDivided)
	{
		tR->CreateNoiseSettings(noiseSettings);
		tL->CreateNoiseSettings(noiseSettings);
		bR->CreateNoiseSettings(noiseSettings);
		bL->CreateNoiseSettings(noiseSettings);
		tR->ReGenerateChunk();	
		tL->ReGenerateChunk();	
		bR->ReGenerateChunk();	
		bL->ReGenerateChunk();
		
	}
}

void AChunk::ToggleMeshType(bool newType)
{
	toggleType = newType;
	generatedMeshVaules = false;
	createdMesh = false;
	CreatePerlinNoiseMesh();

	if (hasDivided)
	{
		tR->ToggleMeshType(toggleType);
		tL->ToggleMeshType(toggleType);
		bR->ToggleMeshType(toggleType);
		bL->ToggleMeshType(toggleType);
	}
}

void AChunk::ToggleMat(bool newType)
{
	toggleMat = newType;
	if (toggleMat) 
	{ 		
		terrainMesh->SetMaterial(0, terrainMat);
	}
	else 
	{ 
		terrainMesh->SetMaterial(0, normalsMat);
	}
	
	if (hasDivided)
	{
		tR->ToggleMat(newType);
		tL->ToggleMat(newType);
		bR->ToggleMat(newType);
		bL->ToggleMat(newType);
	}
}

void AChunk::CreatePerlinNoiseMesh()
{	
	if (toggleType)
	{
		(new FAutoDeleteAsyncTask<HeightMapGeneration>(this, terrainMesh, terrainMat, GetActorLocation(), vertexCount, vertexCountFull, vertexDistance, 2, noiseSettings))->StartBackgroundTask();
	}
	else
	{
		CreateMarchingCubesMesh();
	}
}


void AChunk::UpdateLOD()
{
	if (playerRef == nullptr) { return; }
	if (parent == nullptr && hasDivided == false)
	{
		// chunk at highest level
		//should this chunk subdivide?
		
		float dis = FVector::Distance(playerRef->GetActorLocation(), GetActorLocation());
		if (dis < 10000)
		{
			AddLODChunks();
		}
	}
	else if (parent != nullptr && hasDivided == false)
	{
		if (chunk_LOD >= 3) { return; }
		//has parent and not divided
		float dis = FVector::Distance(playerRef->GetActorLocation(), GetActorLocation());
		if (dis < 10000 / (chunk_LOD * 2))
		{
			AddLODChunks();
		}		
	}

	if (hasDivided == true)
	{
		int LOD_Level = chunk_LOD;
		if (LOD_Level == 0) { LOD_Level = 1; }
		else { LOD_Level = chunk_LOD * 2; }
		float dis = FVector::Distance(playerRef->GetActorLocation(), GetActorLocation());
		if (dis > (10000 / LOD_Level))
		{
			//GLog->Log("Try to remove detial");
			terrainMesh->SetVisibility(true);

			RemoveLOD();
			//GLog->Log("Removing Detail");
			hasDivided = false;
		}
	}
}

void AChunk::RemoveLOD()
{
	if (hasDivided)
	{
		tR->RemoveLOD();
		tL->RemoveLOD();
		bR->RemoveLOD();
		bL->RemoveLOD();

		tR->Destroy();
		tL->Destroy();
		bR->Destroy();
		bL->Destroy();
	}	
}

void AChunk::AddLODChunks()
{
		hasDivided = true;
		float halfChunkSize = ((vertexDistance / 2) * vertexCount) / 2;
		FVector2D center = FVector2D(GetActorLocation().X, GetActorLocation().Y);
		float newVertexDistance = vertexDistance / 2;
		tR = mapRef->SpawnChunkAtPosWithLOD(FVector2D(center.X + halfChunkSize, center.Y + halfChunkSize), chunk_LOD + 1, this, newVertexDistance);
		tL = mapRef->SpawnChunkAtPosWithLOD(FVector2D(center.X + halfChunkSize, center.Y - halfChunkSize), chunk_LOD + 1, this, newVertexDistance);
		bL = mapRef->SpawnChunkAtPosWithLOD(FVector2D(center.X - halfChunkSize, center.Y - halfChunkSize), chunk_LOD + 1, this, newVertexDistance);
		bR = mapRef->SpawnChunkAtPosWithLOD(FVector2D(center.X - halfChunkSize, center.Y + halfChunkSize), chunk_LOD + 1, this, newVertexDistance);
}

void AChunk::CreateMarchingCubesMesh()
{
	if (parent) { zVertexCount = parent->zVertexCount; }
	else {zVertexCount = vertexCount; }

	(new FAutoDeleteAsyncTask<MarchingCubesGeneration>(this,GetActorLocation(), vertexCount, zVertexCount, vertexCountFull, vertexDistance, zVertexDistance, 2,noiseSettings))->StartBackgroundTask();
}

