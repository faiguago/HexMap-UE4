// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGridChunk.h"
#include "Engine/World.h"
#include "HexMesh.h"
#include "HexCell.h"
#include "HexGrid.h"

// Sets default values
AHexGridChunk::AHexGridChunk()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AHexGridChunk::BeginPlay()
{
	Super::BeginPlay();

}

void AHexGridChunk::Init(TSubclassOf<AHexMesh> hexMeshPrefab, UMaterialInstance* TerrainMaterial, UMaterialInstance* WaterMaterial, AHexGrid* hexGrid)
{
	this->hexGrid = hexGrid;
	this->TerrainMaterial = TerrainMaterial;
	this->WaterMaterial = WaterMaterial;

	cells.SetNum(HexMetrics::chunkSize * HexMetrics::chunkSize);

	terrain = GetWorld()->SpawnActor<AHexMesh>(hexMeshPrefab, FTransform());
	terrain->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	terrain->SetMaterial(TerrainMaterial);
	terrain->SetHexGrid(hexGrid);

	water = GetWorld()->SpawnActor<AHexMesh>(hexMeshPrefab, FTransform());
	water->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	water->SetMaterial(WaterMaterial);
	water->SetHexGrid(hexGrid);
}

void AHexGridChunk::AddCell(int32 index, AHexCell* cell)
{
	cells[index] = cell;
	cell->chunk = this;
	cell->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
}

void AHexGridChunk::Refresh()
{
	Triangulate();
}

void AHexGridChunk::Triangulate()
{
	terrain->Clear();
	water->Clear();

	for (int32 i = 0; i < cells.Num(); i++)
	{
		Triangulate(cells[i]);
	}

	terrain->Apply();
	water->Apply();
}

void AHexGridChunk::Triangulate(AHexCell * cell)
{
	for (EHexDirection::Type d = EHexDirection::NE; d <= EHexDirection::NW; d = (EHexDirection::Type)(d + 1))
	{
		Triangulate(d, cell);
	}
}

void AHexGridChunk::Triangulate(EHexDirection::Type direction, AHexCell* cell)
{
	FVector center = cell->GetActorLocation();
	FEdgeVertices e = FEdgeVertices(
		center + HexMetrics::GetFirstSolidCorner(direction),
		center + HexMetrics::GetSecondSolidCorner(direction));

	TriangulateEdgeFan(center, e, cell->GetColor());

	if (direction <= EHexDirection::SE)
	{
		TriangulateConnection(direction, cell, e);
	}

	if (cell->IsUnderWater())
	{
		TriangulateWater(direction, cell, center);
	}
}

void AHexGridChunk::TriangulateWater(EHexDirection::Type direction, AHexCell* cell, const FVector& center)
{
	FVector newCenter = center;
	newCenter.Z = cell->WaterSurfaceZ();

	AHexCell* neighbor = cell->GetNeighbor(direction);
	if (neighbor && !neighbor->IsUnderWater())
	{
		TriangulateWaterShore(direction, cell, neighbor, newCenter);
	}
	else
	{
		TriangulateOpenWater(direction, cell, neighbor, newCenter);
	}
}

void AHexGridChunk::TriangulateOpenWater(EHexDirection::Type direction, AHexCell* cell, AHexCell* neighbor, const FVector& center)
{
	FVector c1 = center + HexMetrics::GetFirstSolidCorner(direction);
	FVector c2 = center + HexMetrics::GetSecondSolidCorner(direction);

	water->AddTriangle(center, c1, c2);

	if (direction <= EHexDirection::SE && neighbor)
	{
		FVector bridge = HexMetrics::GetBridge(direction);
		FVector e1 = c1 + bridge;
		FVector e2 = c2 + bridge;

		water->AddQuad(c1, c2, e1, e2);

		if (direction <= EHexDirection::E)
		{
			AHexCell* nextNeighbor = cell->GetNeighbor(HexDirection::Next(direction));
			if (!nextNeighbor || !nextNeighbor->IsUnderWater())
				return;

			water->AddTriangle(c2, e2, c2 + HexMetrics::GetBridge(HexDirection::Next(direction)));
		}
	}
}

void AHexGridChunk::TriangulateWaterShore(EHexDirection::Type direction, AHexCell* cell, AHexCell* neighbor, const FVector& center)
{
	FEdgeVertices e1 = FEdgeVertices(
		center + HexMetrics::GetFirstSolidCorner(direction),
		center + HexMetrics::GetSecondSolidCorner(direction));

	water->AddTriangle(center, e1.v1, e1.v2);
	water->AddTriangle(center, e1.v2, e1.v3);
	water->AddTriangle(center, e1.v3, e1.v4);

	FVector bridge = HexMetrics::GetBridge(direction);
	FEdgeVertices e2 = FEdgeVertices(
		e1.v1 + bridge,
		e1.v4 + bridge);

	water->AddQuad(e1.v1, e1.v2, e2.v1, e2.v2);
	water->AddQuad(e1.v2, e1.v3, e2.v2, e2.v3);
	water->AddQuad(e1.v3, e1.v4, e2.v3, e2.v4);

	AHexCell* nextNeighbor = cell->GetNeighbor(HexDirection::Next(direction));
	if (nextNeighbor)
	{
		water->AddTriangle(e1.v4, e2.v4, e1.v4 + HexMetrics::GetBridge(HexDirection::Next(direction)));
	}
}

void AHexGridChunk::TriangulateConnection(EHexDirection::Type direction, AHexCell* cell, const FEdgeVertices& e1)
{
	AHexCell* neighbor = cell->GetNeighbor(direction);
	if (!neighbor)
		return;

	FVector bridge = HexMetrics::GetBridge(direction);
	bridge.Z = neighbor->GetActorLocation().Z - cell->GetActorLocation().Z;

	FEdgeVertices e2 = FEdgeVertices(
		e1.v1 + bridge,
		e1.v4 + bridge);

	if (cell->GetEdgeType(direction) == EHexEdge::Slope)
	{
		TriangulateEdgeTerraces(e1, cell, e2, neighbor);
	}
	else
	{
		TriangulateEdgeStrip(e1, cell->GetColor(), e2, neighbor->GetColor());
	}

	AHexCell* nextNeighbor = cell->GetNeighbor(HexDirection::Next(direction));
	if (direction <= EHexDirection::E && nextNeighbor)
	{
		FVector v5 = e1.v4 + HexMetrics::GetBridge(HexDirection::Next(direction));
		v5.Z = nextNeighbor->GetActorLocation().Z;

		if (cell->GetElevation() <= neighbor->GetElevation())
		{
			if (cell->GetElevation() <= nextNeighbor->GetElevation())
			{
				TriangulateCorner(e1.v4, cell, e2.v4, neighbor, v5, nextNeighbor);
			}
			else
			{
				TriangulateCorner(v5, nextNeighbor, e1.v4, cell, e2.v4, neighbor);
			}
		}
		else if (neighbor->GetElevation() <= nextNeighbor->GetElevation())
		{
			TriangulateCorner(e2.v4, neighbor, v5, nextNeighbor, e1.v4, cell);
		}
		else
		{
			TriangulateCorner(v5, nextNeighbor, e1.v4, cell, e2.v4, neighbor);
		}
	}
}

void AHexGridChunk::TriangulateEdgeTerraces(const FEdgeVertices& begin, AHexCell* beginCell,
	const FEdgeVertices& end, AHexCell* endCell)
{
	FEdgeVertices e2 = FEdgeVertices::TerraceLerp(begin, end, 1);
	FLinearColor c2 = HexMetrics::TerraceColorLerp(beginCell->GetColor(), endCell->GetColor(), 1);

	TriangulateEdgeStrip(begin, beginCell->GetColor(), e2, c2);

	for (int32 i = 2; i < HexMetrics::terraceSteps; i++)
	{
		FEdgeVertices e1 = e2;
		FLinearColor c1 = c2;
		e2 = FEdgeVertices::TerraceLerp(begin, end, i);
		c2 = HexMetrics::TerraceColorLerp(beginCell->GetColor(), endCell->GetColor(), i);
		TriangulateEdgeStrip(e1, c1, e2, c2);
	}

	TriangulateEdgeStrip(e2, c2, end, endCell->GetColor());
}

void AHexGridChunk::TriangulateBoundaryTriangle(const FVector& begin, AHexCell* beginCell, const FVector& left, AHexCell* leftCell, const FVector& boundary, const FLinearColor& boundaryColor)
{
	FVector v2 = Perturb(HexMetrics::TerraceLerp(begin, left, 1));
	FLinearColor c2 = HexMetrics::TerraceColorLerp(beginCell->GetColor(), leftCell->GetColor(), 1);

	terrain->AddTriangleUnperturbed(Perturb(begin), v2, boundary);
	terrain->AddTriangleColor(beginCell->GetColor(), c2, boundaryColor);

	for (int32 i = 2; i < HexMetrics::terraceSteps; i++)
	{
		FVector v1 = v2;
		FLinearColor c1 = c2;
		v2 = Perturb(HexMetrics::TerraceLerp(begin, left, i));
		c2 = HexMetrics::TerraceColorLerp(beginCell->GetColor(), leftCell->GetColor(), i);
		terrain->AddTriangleUnperturbed(v1, v2, boundary);
		terrain->AddTriangleColor(c1, c2, boundaryColor);
	}

	terrain->AddTriangleUnperturbed(v2, Perturb(left), boundary);
	terrain->AddTriangleColor(c2, leftCell->GetColor(), boundaryColor);
}

void AHexGridChunk::TriangulateCorner(const FVector& bottom, AHexCell* bottomCell, const FVector& left, AHexCell* leftCell, const FVector& right, AHexCell* rightCell)
{
	EHexEdge::Type leftEdgeType = bottomCell->GetEdgeType(leftCell);
	EHexEdge::Type rightEdgeType = bottomCell->GetEdgeType(rightCell);

	if (leftEdgeType == EHexEdge::Slope)
	{
		if (rightEdgeType == EHexEdge::Slope)
		{
			TriangulateCornerTerraces(bottom, bottomCell, left, leftCell, right, rightCell);
		}
		else if (rightEdgeType == EHexEdge::Flat)
		{
			TriangulateCornerTerraces(left, leftCell, right, rightCell, bottom, bottomCell);

		}
		else
		{
			TriangulateCornerTerracesCliff(bottom, bottomCell, left, leftCell, right, rightCell);
		}
	}
	else if (rightEdgeType == EHexEdge::Slope)
	{
		if (leftEdgeType == EHexEdge::Flat)
		{
			TriangulateCornerTerraces(right, rightCell, bottom, bottomCell, left, leftCell);
		}
		else
		{
			TriangulateCornerCliffTerraces(bottom, bottomCell, left, leftCell, right, rightCell);
		}
	}
	else if (leftCell->GetEdgeType(rightCell) == EHexEdge::Slope)
	{
		if (leftCell->GetElevation() < rightCell->GetElevation())
		{
			TriangulateCornerCliffTerraces(right, rightCell, bottom, bottomCell, left, leftCell);
		}
		else
		{
			TriangulateCornerTerracesCliff(left, leftCell, right, rightCell, bottom, bottomCell);
		}
	}
	else
	{
		terrain->AddTriangle(bottom, left, right);
		terrain->AddTriangleColor(bottomCell->GetColor(), leftCell->GetColor(), rightCell->GetColor());
	}
}

void AHexGridChunk::TriangulateCornerTerraces(const FVector& begin, AHexCell* beginCell, const FVector& left, AHexCell* leftCell, const FVector& right, AHexCell* rightCell)
{
	FVector v3 = HexMetrics::TerraceLerp(begin, left, 1);
	FVector v4 = HexMetrics::TerraceLerp(begin, right, 1);
	FLinearColor c3 = HexMetrics::TerraceColorLerp(beginCell->GetColor(), leftCell->GetColor(), 1);
	FLinearColor c4 = HexMetrics::TerraceColorLerp(beginCell->GetColor(), rightCell->GetColor(), 1);

	terrain->AddTriangle(begin, v3, v4);
	terrain->AddTriangleColor(beginCell->GetColor(), c3, c4);

	for (int32 i = 2; i < HexMetrics::terraceSteps; i++)
	{
		FVector v1 = v3;
		FVector v2 = v4;
		FLinearColor c1 = c3;
		FLinearColor c2 = c4;
		v3 = HexMetrics::TerraceLerp(begin, left, i);
		v4 = HexMetrics::TerraceLerp(begin, right, i);
		c3 = HexMetrics::TerraceColorLerp(beginCell->GetColor(), leftCell->GetColor(), i);
		c4 = HexMetrics::TerraceColorLerp(beginCell->GetColor(), rightCell->GetColor(), i);
		terrain->AddQuad(v1, v2, v3, v4);
		terrain->AddQuadColor(c1, c2, c3, c4);
	}

	terrain->AddQuad(v3, v4, left, right);
	terrain->AddQuadColor(c3, c4, leftCell->GetColor(), rightCell->GetColor());
}

void AHexGridChunk::TriangulateCornerTerracesCliff(const FVector& begin, AHexCell* beginCell, const FVector& left, AHexCell* leftCell, const FVector& right, AHexCell* rightCell)
{
	float b = 1.f / (rightCell->GetElevation() - beginCell->GetElevation());
	b = FMath::Abs(b);

	FVector boundary = FMath::Lerp(Perturb(begin), Perturb(right), b);
	FLinearColor boundaryColor = FMath::Lerp(beginCell->GetColor(), rightCell->GetColor(), b);

	TriangulateBoundaryTriangle(begin, beginCell, left, leftCell, boundary, boundaryColor);

	if (leftCell->GetEdgeType(rightCell) == EHexEdge::Slope)
	{
		TriangulateBoundaryTriangle(left, leftCell, right, rightCell, boundary, boundaryColor);
	}
	else
	{
		terrain->AddTriangleUnperturbed(Perturb(left), Perturb(right), boundary);
		terrain->AddTriangleColor(leftCell->GetColor(), rightCell->GetColor(), boundaryColor);
	}
}

void AHexGridChunk::TriangulateCornerCliffTerraces(const FVector& begin, AHexCell* beginCell, const FVector& left, AHexCell* leftCell, const FVector& right, AHexCell* rightCell)
{
	float b = 1.f / (leftCell->GetElevation() - beginCell->GetElevation());
	b = FMath::Abs(b);

	FVector boundary = FMath::Lerp(Perturb(begin), Perturb(left), b);
	FLinearColor boundaryColor = FMath::Lerp(beginCell->GetColor(), leftCell->GetColor(), b);

	TriangulateBoundaryTriangle(right, rightCell, begin, beginCell, boundary, boundaryColor);

	if (leftCell->GetEdgeType(rightCell) == EHexEdge::Slope)
	{
		TriangulateBoundaryTriangle(left, leftCell, right, rightCell, boundary, boundaryColor);
	}
	else
	{
		terrain->AddTriangleUnperturbed(Perturb(left), Perturb(right), boundary);
		terrain->AddTriangleColor(leftCell->GetColor(), rightCell->GetColor(), boundaryColor);
	}
}

void AHexGridChunk::TriangulateEdgeFan(const FVector& center, const FEdgeVertices& edge, const FLinearColor& color)
{
	terrain->AddTriangle(center, edge.v1, edge.v2);
	terrain->AddTriangleColor(color);
	terrain->AddTriangle(center, edge.v2, edge.v3);
	terrain->AddTriangleColor(color);
	terrain->AddTriangle(center, edge.v3, edge.v4);
	terrain->AddTriangleColor(color);
}

void AHexGridChunk::TriangulateEdgeStrip(const FEdgeVertices& e1, const FLinearColor& c1, const FEdgeVertices& e2, const FLinearColor& c2)
{
	terrain->AddQuad(e1.v1, e1.v2, e2.v1, e2.v2);
	terrain->AddQuadColor(c1, c2);
	terrain->AddQuad(e1.v2, e1.v3, e2.v2, e2.v3);
	terrain->AddQuadColor(c1, c2);
	terrain->AddQuad(e1.v3, e1.v4, e2.v3, e2.v4);
	terrain->AddQuadColor(c1, c2);
}

FVector AHexGridChunk::Perturb(const FVector& position)
{
	FVector pos = position;
	FLinearColor sample = hexGrid->SampleNoise(position);

	pos.X += (sample.R * 2.f - 1.f) * HexMetrics::cellPerturbStrength;
	pos.Y += (sample.G * 2.f - 1.f) * HexMetrics::cellPerturbStrength;

	return pos;
}