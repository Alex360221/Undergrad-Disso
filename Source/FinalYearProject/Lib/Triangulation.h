// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FINALYEARPROJECT_API Triangulation
{
public:
	Triangulation();
	static void CalculateTriangulation(int index, int verticesInLIne, TArray<int>* outArray);
	static void CalculateMarchingCubesTriangulation(FVector cellPos, float cellDistance, int cellsInLine,
		TMap<FVector, float>* cells, TArray<FVector>* vertices, TArray<int>* triangles);
	static void CalculateMarchingCubesTriangulation(FVector cellPos, float cellDistance, float zCellDistance, int cellsInLine,
		TMap<FVector, float>* cells, TArray<FVector>* vertices, TArray<int>* triangles, TArray<FVector2D>* uvs, bool addToNormalIndices, TArray<int>* normalIndices/*, int index,int zxyCount, int xyCount*/);

private:
	static FVector4 CellData(TMap<FVector, float>* cells, FVector cellPos);
	static FVector InterpolateVertices(FVector4 v1, FVector4 v2);
	static void MarchingCubesUnWrap(FVector v1, FVector v2, FVector v3, TArray<FVector2D>* uvs, float uvScale);
};
