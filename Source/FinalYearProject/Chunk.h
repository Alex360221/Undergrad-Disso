// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Chunk.generated.h"


UCLASS()
class FINALYEARPROJECT_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UProceduralMeshComponent* terrainMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UMaterialInstanceDynamic* terrainMat;

	UMaterialInterface* normalsMat;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool hasDivided;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<FVector4> cells;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			float chunkCreationTime;

	FCriticalSection chunkCritialSection;
	TArray<FVector> vertices;
	TArray<int> triangles;
	TArray<FVector2D> UVs;
	TArray<FVector> normals;
	TArray<FProcMeshTangent> Tangents;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Update(float DeltaTime);	//this is my tick function which will get called at a fixed rate
	float vertexDistance;
	int vertexCount;
	int vertexCountFull;
	int zVertexCount;
	int zVertexDistance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int chunk_LOD;
	class APlayerCharacter* playerRef;
	class AMapController* mapRef;
	TArray<class NoiseSettings*> noiseSettings;

	AChunk* parent;

	bool createdMesh;
	bool generatedMeshVaules;
	int meshesCreated;
	bool toggleType;
	bool toggleMat;

	void Start();	//this will start the mesh creation after the varibles have been set
	void CreateNoiseSettings(TArray<class NoiseSettings*> newNoiseSettings);
	void ReGenerateChunk();	//this will regenerate this mesh and it's children
	void ToggleMeshType(bool newType);	//this will toggle the type of mesh this chunk is using
	void ToggleMat(bool newType);//This will toggle the material stuff

	//void CreateMeshSection(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0,
	//	const TArray<FColor>& VertexColors, const TArray<FProcMeshTangent>& Tangents, bool bCreateCollision);
private:
	float timer;
	bool SetVaules;
	int bottomTriPoint, topTriPoint;
	bool marchingCubes;
	
	//tr == x+ y+
	//bl == x- y-
	
	AChunk* tL;
	AChunk* tR;
	AChunk* bL;
	AChunk* bR;

	void CreatePerlinNoiseMesh();
	void UpdateLOD();	//this will workout how far away th
public:
	void RemoveLOD();	//this will remove this chunks LOD and all the chunks below it
	void AddLODChunks();	//this will spawn all the chunks LOD 

	void CreateMarchingCubesMesh();


};
