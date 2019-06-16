// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid.h"
#include "HexCell.h"
#include "Utils/HexMetrics.h"
#include "HexMesh.h"
#include "ProceduralMeshComponent.h"
#include "Utils/HexLibrary.h"
#include "HexGridChunk.h"

// Sets default values
AHexGrid::AHexGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
}

// Called when the game starts or when spawned
void AHexGrid::BeginPlay()
{
	Super::BeginPlay();

	cellCount = chunkCount * HexMetrics::chunkSize;

	CreateChunks();
	CreateCells();

	for (int32 i = 0; i < chunks.Num(); i++)
		chunks[i]->Refresh();
}

void AHexGrid::CreateChunks()
{
	for (int32 x = 0; x < chunkCount; x++)
	{
		for (int32 y = 0; y < chunkCount; y++)
		{
			AHexGridChunk* chunk = GetWorld()->SpawnActor<AHexGridChunk>(chunkPrefab, FTransform());
			chunk->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
			chunk->Init(hexMeshPrefab, TerrainMaterial, WaterMaterial, this);
			chunks.Add(chunk);
		}
	}
}

void AHexGrid::CreateCells()
{
	for (int32 x = 0, i = 0; x < cellCount; x++)
	{
		for (int32 y = 0; y < cellCount; y++)
		{
			CreateCell(x, y, i++);
		}
	}
}

void AHexGrid::CreateCell(int32 x, int32 y, int32 i)
{
	FVector position = FVector(
		x * HexMetrics::outerRadius * 1.5f,
		(y + x * 0.5f - x / 2) * HexMetrics::innerRadius * 2.f,
		0) * HexMetrics::hexScale;

	AHexCell * cell = GetWorld()->SpawnActor<AHexCell>(cellPrefab, position, FRotator(0));
	cell->coordinates = FHexCoordinates::FromOffsetCoordinates(y, x);
	cell->SetActorLabel(FString::FromInt(i));
	cell->Initialize(this);

	//cell->UpdateText(cell->coordinates.ToString());
	//cell->UpdateText(FString::FromInt(i));
	cell->UpdateText("");

	if (y > 0)
	{
		cell->SetNeighbor(EHexDirection::W, cells[i - 1]);
	}
	if (x > 0)
	{
		if ((x & 1) == 0)
		{
			cell->SetNeighbor(EHexDirection::SE, cells[i - cellCount]);
			if (y > 0)
			{
				cell->SetNeighbor(EHexDirection::SW, cells[i - cellCount - 1]);
			}
		}
		else
		{
			cell->SetNeighbor(EHexDirection::SW, cells[i - cellCount]);
			if (y < cellCount - 1)
			{
				cell->SetNeighbor(EHexDirection::SE, cells[i - cellCount + 1]);
			}
		}
	}

	cells.Add(cell);
	cells[i]->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

	cell->SetElevation(0);

	AddCellToChunk(x, y, cell);
}

void AHexGrid::AddCellToChunk(int32 x, int32 y, AHexCell* cell)
{
	int32 chunkX = x / HexMetrics::chunkSize;
	int32 chunkY = y / HexMetrics::chunkSize;

	int32 index = chunkX + chunkY * chunkCount;

	AHexGridChunk* chunk = chunks[index];

	int32 localX = x - chunkX * HexMetrics::chunkSize;
	int32 localY = y - chunkY * HexMetrics::chunkSize;

	chunk->AddCell(localX + localY * HexMetrics::chunkSize, cell);
}

void AHexGrid::UpdateCell(const FVector& position, const FLinearColor& newColor, bool updateTerrain, int32 terrainElevation, bool updateWater, int32 waterElevation, int32 brushSize)
{
	this->updateTerrain = updateTerrain;
	this->updateWater = updateWater;

	FVector localPos = GetTransform().InverseTransformPosition(position);
	FHexCoordinates coordinates = FHexCoordinates::FromPosition(localPos);

	int32 centerX = coordinates.X;
	int32 centerZ = coordinates.Z;

	for (int32 r = 0, z = centerZ - brushSize; z <= centerZ; z++, r++)
	{
		for (int32 x = centerX - r; x <= centerX + brushSize; x++)
		{
			UpdateCell(x, z, newColor, terrainElevation, waterElevation);
		}
	}
	for (int32 r = 0, z = centerZ + brushSize; z > centerZ; z--, r++)
	{
		for (int32 x = centerX - brushSize; x <= centerX + r; x++)
		{
			UpdateCell(x, z, newColor, terrainElevation, waterElevation);
		}
	}
}

void AHexGrid::UpdateCell(int32 x, int32 z, const FLinearColor& newColor, int32 terrainElevation, int32 waterElevation)
{
	FHexCoordinates coord = FHexCoordinates(x, z);

	z = coord.Z;
	x = coord.X + z / 2;

	if (x < 0 || x >= cellCount
		|| z < 0 || x >= cellCount)
		return;

	int32 index = x + z * cellCount;

	if (!cells.IsValidIndex(index))
		return;

	if (updateTerrain)
	{
		cells[index]->SetColor(newColor);
		cells[index]->SetElevation(terrainElevation);
	}

	if (updateWater)
		cells[index]->SetWaterLevel(waterElevation);
}

FLinearColor AHexGrid::SampleNoise(const FVector& position)
{
	FVector pos = position * noiseScale;
	return UHexLibrary::GetBilinearPixel(noiseSource, pos.X, pos.Y);
}