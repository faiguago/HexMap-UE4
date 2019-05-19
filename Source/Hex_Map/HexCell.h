// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utils/HexMetrics.h"
#include "HexCell.generated.h"

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
	
public:

	float GetElevation();

	void SetElevation(const float& elevation);

	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* textComponent;

	FHexCoordinates coordinates;

	FLinearColor color;

	void UpdateText(const FString& newText);

	AHexCell* GetNeighbor(EHexDirection::Type direction);

	void SetNeighbor(EHexDirection::Type direction, AHexCell* cell);

	void Initialize();

private:

	float elevation;
	
	float orTextElevation;

	UPROPERTY(VisibleAnywhere)
	TArray<AHexCell*> neighbors;

};
