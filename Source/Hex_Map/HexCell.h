// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utils/HexMetrics.h"
#include "HexCell.generated.h"

class AHexGrid;
class AHexGridChunk;
class UTextRenderComponent;

UCLASS()
class HEX_MAP_API AHexCell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	int32 waterLevel;
	
public:

	float RiverSurfaceZ() const;

	float WaterSurfaceZ() const;

	int32 GetElevation() const;

	void SetElevation(int32 elevation);

	bool IsUnderWater() const;

	int32 GetWaterLevel() const;

	void SetWaterLevel(int32 waterLevel);

	const FLinearColor& GetColor();

	void SetColor(const FLinearColor& color);

	UPROPERTY()
	AHexGridChunk* chunk;

	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* textComponent;

	FHexCoordinates coordinates;
	
	void UpdateText(const FString& newText);

	AHexCell* GetNeighbor(EHexDirection::Type direction);

	void SetNeighbor(EHexDirection::Type direction, AHexCell* cell);

	void Initialize(AHexGrid* hexGrid);

	EHexEdge::Type GetEdgeType(EHexDirection::Type direction);

	EHexEdge::Type GetEdgeType(AHexCell* otherCell);

private:

	int32 elevation = -1;

	FLinearColor color = FLinearColor(1, 0, 0, 0);
	
	float orTextElevation;

	UPROPERTY(VisibleAnywhere)
	TArray<AHexCell*> neighbors;

	UPROPERTY()
	AHexGrid* hexGrid;

	void Refresh();
};