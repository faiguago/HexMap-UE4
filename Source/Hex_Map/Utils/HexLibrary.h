// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HexLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HEX_MAP_API UHexLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = HexMap)
	static FVector HexCoordinatesFromPosition(FVector position);
};
