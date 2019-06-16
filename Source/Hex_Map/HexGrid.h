// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Texture2D.h"
#include "HexGrid.generated.h"

class AHexCell;
class AHexMesh;
class UMaterialInstance;
class AHexGridChunk;

UCLASS()
class HEX_MAP_API AHexGrid : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHexGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
		int32 chunkCount = 3;

	UPROPERTY(EditAnywhere)
		float noiseScale = 0.3f;

	UPROPERTY(EditAnywhere)
		UMaterialInstance * TerrainMaterial;

	UPROPERTY(EditAnywhere)
		UMaterialInstance * WaterMaterial;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AHexCell> cellPrefab;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AHexMesh> hexMeshPrefab;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AHexGridChunk> chunkPrefab;

private:

	bool updateTerrain, updateWater;

	int32 cellCount;

	UPROPERTY()
		TArray<AHexCell*> cells;

	UPROPERTY()
		TArray<AHexGridChunk*> chunks;

	UPROPERTY(EditAnywhere)
		UTexture2D* noiseSource;

	void CreateChunks();

	void CreateCells();

	void CreateCell(int32 x, int32 y, int32 i);

	void AddCellToChunk(int32 x, int32 y, AHexCell* cell);

public:

	UFUNCTION(BlueprintCallable, Category = HexMap)
		void UpdateCell(const FVector& position, const FLinearColor& newColor, bool updateTerrain, int32 terrainElevation, bool updateWater, int32 waterElevation, int32 brushSize);

	void UpdateCell(int32 x, int32 z, const FLinearColor& newColor, int32 terrainElevation, int32 waterElevation);

	FLinearColor SampleNoise(const FVector& position);

};