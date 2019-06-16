// Fill out your copyright notice in the Description page of Project Settings.


#include "HexCell.h"
#include "Components/TextRenderComponent.h"
#include "HexGrid.h"
#include "HexGridChunk.h"

// Sets default values
AHexCell::AHexCell()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComp");

	textComponent = CreateDefaultSubobject<UTextRenderComponent>("TextComp");
	textComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHexCell::BeginPlay()
{
	Super::BeginPlay();

	orTextElevation = textComponent->GetComponentTransform().GetLocation().Z;
}

float AHexCell::RiverSurfaceZ() const
{
	return (elevation + HexMetrics::waterElevationOffset)
		* HexMetrics::elevationStep;
}

float AHexCell::WaterSurfaceZ() const
{
	return (waterLevel + HexMetrics::waterElevationOffset)
		* HexMetrics::elevationStep;
}

int32 AHexCell::GetElevation() const
{
	return elevation;
}

bool AHexCell::IsUnderWater() const
{
	return waterLevel > elevation;
}

int32 AHexCell::GetWaterLevel() const
{
	return waterLevel;
}

void AHexCell::SetWaterLevel(int32 waterLevel)
{
	if (this->waterLevel == waterLevel)
		return;

	this->waterLevel = waterLevel;
	Refresh();
}

void AHexCell::SetElevation(int32 elevation)
{
	if (this->elevation == elevation)
		return;

	this->elevation = elevation;

	FVector position = GetActorLocation();
	position.Z = elevation * HexMetrics::elevationStep;
	position.Z += (hexGrid->SampleNoise(position).G * 2.f - 1.f) * HexMetrics::elevationPerturbStrength;
	SetActorLocation(position);

	FVector textPos = textComponent->GetComponentTransform().GetLocation();
	textPos.Z = orTextElevation + elevation * HexMetrics::elevationStep;
	textComponent->SetWorldLocation(textPos);

	Refresh();
}

const FLinearColor& AHexCell::GetColor()
{
	return color;
}

void AHexCell::SetColor(const FLinearColor& color)
{
	if (this->color == color)
		return;

	this->color = color;
	Refresh();
}

void AHexCell::UpdateText(const FString& newText)
{
	textComponent->SetText(newText);
}

AHexCell* AHexCell::GetNeighbor(EHexDirection::Type direction)
{
	return neighbors[(int32)direction];
}

void AHexCell::SetNeighbor(EHexDirection::Type direction, AHexCell* cell)
{
	neighbors[direction] = cell;
	EHexDirection::Type opposite = HexDirection::Opposite(direction);
	cell->neighbors[opposite] = this;
}

void AHexCell::Initialize(AHexGrid* hexGrid)
{
	neighbors.SetNum(6);
	this->hexGrid = hexGrid;
}

EHexEdge::Type AHexCell::GetEdgeType(EHexDirection::Type direction)
{
	return HexMetrics::GetEdgeType(
		elevation, neighbors[direction]->elevation);
}

EHexEdge::Type AHexCell::GetEdgeType(AHexCell* otherCell)
{
	return HexMetrics::GetEdgeType(
		elevation, otherCell->GetElevation());
}

void AHexCell::Refresh()
{
	if (!chunk)
		return;

	TArray<AHexGridChunk*> chunks;
	chunks.AddUnique(chunk);

	for (int32 i = 0; i < neighbors.Num(); i++)
	{
		AHexCell* neighbor = neighbors[i];
		if (neighbor)
			chunks.AddUnique(neighbor->chunk);
	}

	for (int32 i = 0; i < chunks.Num(); i++)
		chunks[i]->Refresh();
}