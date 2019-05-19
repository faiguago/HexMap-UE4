// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexGrid.generated.h"

class AHexCell;
class AHexMesh;
class UMaterialInstance;

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

	UPROPERTY(EditAnywhere)
	int width = 6;

	UPROPERTY(EditAnywhere)
	int height = 6;

	UPROPERTY(EditAnywhere)
	UMaterialInstance * Material;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AHexCell> cellPrefab;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AHexMesh> hexMeshPrefab;

	UPROPERTY(EditAnywhere)
	FLinearColor DefaultColor;

	UPROPERTY(EditAnywhere)
	FLinearColor TouchedColor;
	
private:

	UPROPERTY()
	AHexMesh * hexMesh;
	
	UPROPERTY()
	TArray<AHexCell*> cells;

	void CreateCell(int32 x, int32 y, int32 i);

	void Refresh();

public:

	UFUNCTION(BlueprintCallable, Category = HexMap)
	void UpdateCell(const FVector& position, const FLinearColor& newColor, const float& newElevation);

};
