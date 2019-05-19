// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid.h"
#include "HexCell.h"
#include "Utils/HexMetrics.h"
#include "HexMesh.h"
#include "ProceduralMeshComponent.h"

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
	
	for (int32 x = 0, i = 0; x < width; x++)
	{
		for (int32 y = 0; y < height; y++)
		{
			CreateCell(x, y, i++);
		}
	}

	hexMesh = GetWorld()->SpawnActor<AHexMesh>(hexMeshPrefab, FTransform());
	hexMesh->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

	hexMesh->Triangulate(cells);
	hexMesh->SetMaterial(Material);
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
	cell->Initialize();

	cell->color = DefaultColor;

	//cell->UpdateText(cell->coordinates.ToString());
	cell->UpdateText(FString::FromInt(i));
	//cell->UpdateText("");

	if (y > 0)
	{
		cell->SetNeighbor(EHexDirection::W, cells[i - 1]);
	}
	if (x > 0)
	{
		if ((x & 1) == 0)
		{
			cell->SetNeighbor(EHexDirection::SE, cells[i - width]);
			if (y > 0)
			{
				cell->SetNeighbor(EHexDirection::SW, cells[i - width - 1]);
			}
		}
		else
		{
			cell->SetNeighbor(EHexDirection::SW, cells[i - width]);
			if (y < width - 1)
			{
				cell->SetNeighbor(EHexDirection::SE, cells[i - width + 1]);
			}
		}
	}
		
	cells.Add(cell);
	cells[i]->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
}

void AHexGrid::UpdateCell(const FVector& position, const FLinearColor& newColor, const float& newElevation)
{
	FVector localPos = GetTransform().InverseTransformPosition(position);
	
	FHexCoordinates coordinates = FHexCoordinates::FromPosition(localPos);
	int32 index = coordinates.X + coordinates.Z * width + coordinates.Z / 2;
		
	if (!cells.IsValidIndex(index))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Cell Index!"));
		return;
	}

	cells[index]->color = newColor;
	cells[index]->SetElevation(newElevation);

	Refresh();
}

void AHexGrid::Refresh()
{
	hexMesh->Triangulate(cells);
}