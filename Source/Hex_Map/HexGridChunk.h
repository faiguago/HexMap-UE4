// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utils/HexMetrics.h"
#include "HexGridChunk.generated.h"

class AHexCell;
class AHexMesh;
class AHexGrid;

UCLASS()
class HEX_MAP_API AHexGridChunk : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHexGridChunk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY()
		AHexMesh* terrain;

	UPROPERTY()
		AHexMesh* water;

	void Init(TSubclassOf<AHexMesh> hexMeshPrefab, UMaterialInstance* TerrainMaterial, UMaterialInstance* WaterMaterial, AHexGrid* hexGrid);

	void AddCell(int32 chunkSizeX, AHexCell* cell);

	void Refresh();

private:

	UPROPERTY()
		TArray<AHexCell*> cells;

	UPROPERTY()
		UMaterialInstance* TerrainMaterial;

	UPROPERTY()
		UMaterialInstance* WaterMaterial;

	UPROPERTY()
		AHexGrid* hexGrid;

	UPROPERTY()
		TSubclassOf<AHexMesh> hexMeshPrefab;

	void TriangulateWater(EHexDirection::Type direction, AHexCell* cell, const FVector& center);

	void TriangulateOpenWater(EHexDirection::Type direction, AHexCell* cell, AHexCell* neighbor, const FVector& center);

	void TriangulateWaterShore(EHexDirection::Type direction, AHexCell* cell, AHexCell* neighbor, const FVector& center);

private:

	void Triangulate();

	void TriangulateCorner(const FVector& bottom, AHexCell* bottomCell, const FVector& left, AHexCell* leftCell,
		const FVector& right, AHexCell* rightCell);

	void TriangulateCornerTerraces(const FVector& begin, AHexCell* beginCell, const FVector& left, AHexCell* leftCell,
		const FVector& right, AHexCell* rightCell);

	void TriangulateCornerTerracesCliff(const FVector& begin, AHexCell* beginCell, const FVector& left, AHexCell* leftCell,
		const FVector& right, AHexCell* rightCell);

	void TriangulateBoundaryTriangle(const FVector& begin, AHexCell* beginCell, const FVector& left, AHexCell* leftCell,
		const FVector& boundary, const FLinearColor& boundaryColor);

	void TriangulateCornerCliffTerraces(const FVector& begin, AHexCell* beginCell, const FVector& left, AHexCell* leftCell,
		const FVector& right, AHexCell* rightCell);

	void TriangulateEdgeFan(const FVector& center, const FEdgeVertices& edge, const FLinearColor& color);

	void TriangulateEdgeStrip(const FEdgeVertices& e1, const FLinearColor& c1, const FEdgeVertices& e2, const FLinearColor& c2);

	void Triangulate(AHexCell * cell);

	void Triangulate(EHexDirection::Type direction, AHexCell* cell);

	void TriangulateConnection(EHexDirection::Type direction, AHexCell* cell, const FEdgeVertices& e1);

	void TriangulateEdgeTerraces(const FEdgeVertices& begin, AHexCell* beginCell,
		const FEdgeVertices& end, AHexCell* endCell);

	FVector Perturb(const FVector& position);
};
