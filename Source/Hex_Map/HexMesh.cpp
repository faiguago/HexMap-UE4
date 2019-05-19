// Fill out your copyright notice in the Description page of Project Settings.


#include "HexMesh.h"
#include "ProceduralMeshComponent.h"
#include "HexCell.h"
#include "Utils/HexMetrics.h"

// Sets default values
AHexMesh::AHexMesh()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	hexMesh = CreateDefaultSubobject<UProceduralMeshComponent>("HexMesh");
	SetRootComponent(hexMesh);
}

// Called when the game starts or when spawned
void AHexMesh::BeginPlay()
{
	Super::BeginPlay();

}

void AHexMesh::Triangulate(const TArray<AHexCell*> & cells)
{
	hexMesh->ClearAllMeshSections();

	vertices.Empty();
	normals.Empty();
	triangles.Empty();
	colors.Empty();

	for (int32 i = 0; i < cells.Num(); i++)
	{
		Triangulate(cells[i]);
	}

	hexMesh->CreateMeshSection_LinearColor(0, vertices, triangles,
		normals, TArray<FVector2D>(), colors,
		TArray<FProcMeshTangent>(), true);
}

void AHexMesh::Triangulate(AHexCell * cell)
{
	for (EHexDirection::Type d = EHexDirection::NE; d <= EHexDirection::NW; d = (EHexDirection::Type)(d + 1))
	{
		Triangulate(d, cell);
	}
}

void AHexMesh::Triangulate(EHexDirection::Type direction, AHexCell* cell)
{
	FVector center = cell->GetActorLocation();

	FVector v1 = center + HexMetrics::GetFirstSolidCorner(direction);
	FVector v2 = center + HexMetrics::GetSecondSolidCorner(direction);

	AddTriangle(center, v1, v2);
	AddTriangleColor(cell->color);

	if (direction <= EHexDirection::SE)
	{
		TriangulateConnection(direction, cell, v1, v2);
	}
}

void AHexMesh::TriangulateConnection(EHexDirection::Type direction, AHexCell* cell, const FVector& v1, const FVector& v2)
{
	AHexCell* neighbor = cell->GetNeighbor(direction);
	if (!neighbor)
	{
		//UE_LOG(LogTemp, Error, TEXT("%s"), *cell->coordinates.ToString());
		return;
	}

	FVector bridge = HexMetrics::GetBridge(direction);
	FVector v3 = v1 + bridge;
	FVector v4 = v2 + bridge;
	v3.Z = v4.Z = neighbor->GetElevation() * HexMetrics::elevationStep;

	AddQuad(v1, v2, v3, v4);
	AddQuadColor(cell->color, neighbor->color);

	AHexCell* nextNeighbor = cell->GetNeighbor(HexDirection::Next(direction));
	if (direction <= EHexDirection::E && nextNeighbor)
	{
		FVector v5 = v2 + HexMetrics::GetBridge(HexDirection::Next(direction));
		v5.Z = nextNeighbor->GetElevation() * HexMetrics::elevationStep;
		AddTriangle(v2, v4, v5);
		AddTriangleColor(cell->color, neighbor->color, nextNeighbor->color);
	}
}

void AHexMesh::SetMaterial(UMaterialInstance* Material)
{
	hexMesh->SetMaterial(0, Material);
}

void AHexMesh::AddTriangle(const FVector & v1, const FVector & v2, const FVector & v3)
{
	int32 vertexIndex = vertices.Num();

	vertices.Add(v1);
	vertices.Add(v2);
	vertices.Add(v3);

	normals.Add(FVector::UpVector);
	normals.Add(FVector::UpVector);
	normals.Add(FVector::UpVector);

	triangles.Add(vertexIndex);
	triangles.Add(vertexIndex + 1);
	triangles.Add(vertexIndex + 2);
}

void AHexMesh::AddTriangleColor(const FLinearColor& color)
{
	colors.Add(color);
	colors.Add(color);
	colors.Add(color);
}

void AHexMesh::AddTriangleColor(const FLinearColor& c1, const FLinearColor& c2, const FLinearColor& c3)
{
	colors.Add(c1);
	colors.Add(c2);
	colors.Add(c3);
}

void AHexMesh::AddQuad(const FVector& v1, const FVector& v2, const FVector& v3, const FVector& v4)
{
	int32 vertexIndex = vertices.Num();

	vertices.Add(v1);
	vertices.Add(v2);
	vertices.Add(v3);
	vertices.Add(v4);

	triangles.Add(vertexIndex);
	triangles.Add(vertexIndex + 2);
	triangles.Add(vertexIndex + 1);
	triangles.Add(vertexIndex + 1);
	triangles.Add(vertexIndex + 2);
	triangles.Add(vertexIndex + 3);
}

void AHexMesh::AddQuadColor(const FLinearColor& c1, const FLinearColor& c2, const FLinearColor& c3, const FLinearColor& c4)
{
	colors.Add(c1);
	colors.Add(c2);
	colors.Add(c3);
	colors.Add(c4);
}

void AHexMesh::AddQuadColor(const FLinearColor& c1, const FLinearColor& c2)
{
	colors.Add(c1);
	colors.Add(c1);
	colors.Add(c2);
	colors.Add(c2);
}
