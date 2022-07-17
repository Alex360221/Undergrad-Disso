// Fill out your copyright notice in the Description page of Project Settings.


#include "MapController.h"
#include "Chunk.h"
#include "PlayerCharacter.h"

// Sets default values
AMapController::AMapController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	vertexDistance = 400;
	vertexCount = 16;
	chunkSize = vertexDistance * 16;
	playerViewDistance = 5;
	toggleMesh = true;
	toggleMat = true;

	masterMat = CreateDefaultSubobject<UMaterialInterface>(TEXT("Terrain Mat"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Material1(TEXT("MaterialInstanceConstant'/Game/Mats/TerrainMat_Inst.TerrainMat_Inst'"));
	masterMat = Material1.Object;

	
}

// Called when the game starts or when spawned
void AMapController::BeginPlay()
{
	Super::BeginPlay();
	terrainMat = UMaterialInstanceDynamic::Create(masterMat, this);
	noiseSettings.Add(new NoiseSettings());//adds deafult noise settings to array
	
}

// Called every frame
void AMapController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timer += DeltaTime;
	if (timer >= 0.4f)
	{
		timer = 0.0f;
		TArray<AChunk*> c;
		chunks.GenerateValueArray(c);
		for (int i = 0; i < c.Num();i++)
		{
			c[i]->Update(DeltaTime);
		}		
	}

	if (playerRef != nullptr)
	{
		SpawnChunks(playerRef->GetActorLocation());
		RemoveChunks(playerRef->GetActorLocation());
	}	
}

void AMapController::ReGenerateChunks()
{
	noiseSettings[0]->strength++;
	TArray<AChunk*> loopChunks;
	chunks.GenerateValueArray(loopChunks);
	for (int i = 0; i < loopChunks.Num();i++)
	{
		loopChunks[i]->CreateNoiseSettings(noiseSettings);
		loopChunks[i]->ReGenerateChunk();
	}
}

void AMapController::ToggleMeshType()
{
	if (toggleMesh) { toggleMesh = false; }
	else { toggleMesh = true; }
	TArray<AChunk*> loopChunks;
	chunks.GenerateValueArray(loopChunks);
	for (int i = 0; i < loopChunks.Num();i++)
	{
		loopChunks[i]->ToggleMeshType(toggleMesh);
	}
}

void AMapController::ToggleMat()
{
	if (toggleMat) { toggleMat = false; }
	else { toggleMat = true; }
	TArray<AChunk*> loopChunks;
	chunks.GenerateValueArray(loopChunks);
	for (int i = 0; i < loopChunks.Num();i++)
	{
		loopChunks[i]->ToggleMat(toggleMat);
	}
}

void AMapController::SpawnChunks(FVector playerPos)
{
	//convert player pos to chunk pos
	FVector2D chunkPlayerPos = FVector2D(int(playerPos.X / chunkSize), int(playerPos.Y / chunkSize));
	FVector2D chunkPos = FVector2D::ZeroVector;
	//loop through all the normal biggest size chunks
	for (int x = (playerViewDistance * -1) + chunkPlayerPos.X;x <= (playerViewDistance + chunkPlayerPos.X);x++)
	{
		chunkPos.X = x * chunkSize;
		for (int y = (playerViewDistance * -1) + chunkPlayerPos.Y;y <= (playerViewDistance + chunkPlayerPos.Y);y++)
		{
			chunkPos.Y = y * chunkSize;
			if (chunks.Contains(chunkPos) == false)
			{
				if(WithinRenderDistance(chunkPos, playerPos)) { SpawnChunk(chunkPos); }				
			}
		}
	}
}

void AMapController::SpawnChunk(FVector2D chunkPos)
{
	FVector newChunkPos = FVector(chunkPos.X, chunkPos.Y, 0);
	FRotator rot = FRotator::ZeroRotator;
	AChunk* chunk = GetWorld()->SpawnActor<AChunk>(newChunkPos, rot);
	chunks.Add(chunkPos, chunk);
	if (chunk != nullptr)
	{
		chunk->mapRef = this;
		chunk->CreateNoiseSettings(noiseSettings);
		chunk->vertexCount = vertexCount;
		chunk->vertexDistance = vertexDistance;
		chunk->playerRef = playerRef;
		chunk->toggleType = toggleMesh;
		chunk->toggleMat = toggleMat;
		//chunk->SetActorTickEnabled(true);
		chunk->terrainMat = terrainMat;
		chunk->Start();		
	}
	//GLog->Log("Spawn Chunk!!");
}

void AMapController::RemoveChunks(FVector playerPos)
{
	////convert player pos to chunk pos
	TArray<FVector2D> chunksPos;
	chunks.GenerateKeyArray(chunksPos);
	//check if chunk is outside area
	for (int i = 0; i < chunksPos.Num();i++)
	{
		FVector2D checkChunk = chunksPos[i];
		if (!WithinRenderDistance(checkChunk,playerPos))
		{
			RemoveChunk(checkChunk, chunks.FindRef(checkChunk));
		}
	}
}

void AMapController::RemoveChunk(FVector2D chunkPos, AChunk* chunk)
{
	chunk->Destroy();
	chunks.Remove(chunkPos);
}

bool AMapController::WithinRenderDistance(FVector2D chunkPos, FVector playerPos)
{
	float renderDis = chunkSize * playerViewDistance;
	float chunkDis = FVector2D::Distance(chunkPos, FVector2D(playerPos.X, playerPos.Y));
	if (chunkDis < renderDis) { return true; }
	return false;
}

AChunk* AMapController::SpawnChunkAtPosWithLOD(FVector2D chunkPos, int LOD, AChunk* parent, float newVertexDistance)
{
	FVector newChunkPos = FVector(chunkPos.X, chunkPos.Y, 0);
	FRotator rot = FRotator::ZeroRotator;
	AChunk* chunk = GetWorld()->SpawnActor<AChunk>(newChunkPos, rot);
	if (chunk != nullptr)
	{
		chunk->parent = parent;
		chunk->chunk_LOD = LOD;
		chunk->mapRef = this;
		chunk->toggleType = toggleMesh;
		chunk->toggleMat = toggleMat;
		chunk->CreateNoiseSettings(noiseSettings);
		chunk->vertexCount = vertexCount;
		chunk->vertexDistance = newVertexDistance;
		chunk->playerRef = playerRef;
		chunk->terrainMat = terrainMat;
		chunk->Start();
		return chunk;
	}
	return nullptr;
}

AChunk* AMapController::SpawnChunkAtPosWithLOD(FVector chunkPos, int LOD, AChunk* parent, float newVertexDistance)
{
	FVector newChunkPos = FVector(chunkPos.X, chunkPos.Y, 0);
	FRotator rot = FRotator::ZeroRotator;
	AChunk* chunk = GetWorld()->SpawnActor<AChunk>(newChunkPos, rot);
	if (chunk != nullptr)
	{
		chunk->parent = parent;
		chunk->chunk_LOD = LOD;
		chunk->mapRef = this;
		chunk->toggleType = toggleMesh;
		chunk->toggleMat = toggleMat;
		chunk->CreateNoiseSettings(noiseSettings);
		chunk->vertexCount = vertexCount;
		chunk->vertexDistance = newVertexDistance;
		chunk->playerRef = playerRef;
		chunk->Start();
		chunk->terrainMat = terrainMat;
		return chunk;
	}
	return nullptr;
}

