// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utils/HexMetrics.h"
#include "Materials/MaterialInstance.h"
#include "HexMesh.generated.h"

class UProceduralMeshComponent;
class AHexCell;

UCLASS()
class HEX_MAP_API AHexMesh : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHexMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent * hexMesh;

	UPROPERTY()
	TArray<FVector> vertices;

	UPROPERTY()
	TArray<FVector> normals;

	UPROPERTY()
	TArray<int> triangles;

	UPROPERTY()
	TArray<FLinearColor> colors;

public:

	void Triangulate(const TArray<AHexCell*> & cells);

	void SetMaterial(UMaterialInstance* Material);

private:
	
	void Triangulate(AHexCell * cell);

	void Triangulate(EHexDirection::Type direction, AHexCell* cell);

	void TriangulateConnection(EHexDirection::Type direction, AHexCell* cell, const FVector& v1, const FVector& v2);

	void AddTriangle(const FVector& v1, const FVector& v2, const FVector& sv3);

	void AddTriangleColor(const FLinearColor& color);

	void AddTriangleColor(const FLinearColor& c1, const FLinearColor& c2, const FLinearColor& c3);

	void AddQuad(const FVector& v1, const FVector& v2, const FVector& v3, const FVector& v4);

	void AddQuadColor(const FLinearColor& c1, const FLinearColor& c2, const FLinearColor& c3, const FLinearColor& c4);

	void AddQuadColor(const FLinearColor& c1, const FLinearColor& c2);

};
