// Fill out your copyright notice in the Description page of Project Settings.


#include "HexMesh.h"
#include "ProceduralMeshComponent.h"
#include "HexCell.h"
#include "Utils/HexMetrics.h"
#include "HexGrid.h"

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

void AHexMesh::Clear()
{
	hexMesh->ClearAllMeshSections();

	vertices.Empty();
	normals.Empty();
	triangles.Empty();
	colors.Empty();
}

void AHexMesh::Apply()
{
	RecalculateNormals();

	hexMesh->CreateMeshSection_LinearColor(0, vertices, triangles,
		normals, TArray<FVector2D>(), colors,
		TArray<FProcMeshTangent>(), true);
}

void AHexMesh::SetHexGrid(AHexGrid* hexGrid)
{
	this->hexGrid = hexGrid;
}

void AHexMesh::SetMaterial(UMaterialInstance* Material)
{
	hexMesh->SetMaterial(0, Material);
}


void AHexMesh::AddTriangle(const FVector & v1, const FVector & v2, const FVector & v3)
{
	int32 vertexIndex = vertices.Num();

	vertices.Add(Perturb(v1));
	vertices.Add(Perturb(v2));
	vertices.Add(Perturb(v3));

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

	vertices.Add(Perturb(v1));
	vertices.Add(Perturb(v2));
	vertices.Add(Perturb(v3));
	vertices.Add(Perturb(v4));

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

void AHexMesh::AddTriangleUnperturbed(const FVector& v1, const FVector& v2, const FVector& v3)
{
	int32 vertexIndex = vertices.Num();
	vertices.Add(v1);
	vertices.Add(v2);
	vertices.Add(v3);
	triangles.Add(vertexIndex);
	triangles.Add(vertexIndex + 1);
	triangles.Add(vertexIndex + 2);
}

FVector AHexMesh::Perturb(const FVector& position)
{
	FVector pos = position;
	FLinearColor sample = hexGrid->SampleNoise(position);

	pos.X += (sample.R * 2.f - 1.f) * HexMetrics::cellPerturbStrength;
	pos.Y += (sample.G * 2.f - 1.f) * HexMetrics::cellPerturbStrength;

	return pos;
}

void AHexMesh::RecalculateNormals()
{
	normals.Init(FVector::ZeroVector, vertices.Num());

	for (int32 i = 0; i < triangles.Num(); i += 3)
	{
		FVector normal = CalculateNormal(
			vertices[triangles[i]],
			vertices[triangles[i + 1]],
			vertices[triangles[i + 2]]);

		normals[triangles[i]] += normal;
		normals[triangles[i + 1]] += normal;
		normals[triangles[i + 2]] += normal;
	}

	for (int32 i = 0; i < normals.Num(); i++)
	{
		normals[i] = normals[i].GetSafeNormal();
	}
}

FVector AHexMesh::CalculateNormal(const FVector& A, const FVector& B, const FVector& C)
{
	return FVector::CrossProduct((C - A).GetSafeNormal(), (B - A).GetSafeNormal());
}
