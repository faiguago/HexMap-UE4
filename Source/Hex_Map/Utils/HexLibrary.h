// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HexLibrary.generated.h"

class UTexture2D;

/**
 * 
 */
UCLASS()
class HEX_MAP_API UHexLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = HexMap)
	static FVector HexCoordinatesFromPosition(FVector position);

	UFUNCTION(BlueprintCallable, Category = HexMap)
	static FLinearColor GetBilinearPixel(UTexture2D* tex, float u, float v);
};
