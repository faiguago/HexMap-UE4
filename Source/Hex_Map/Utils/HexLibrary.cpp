// Fill out your copyright notice in the Description page of Project Settings.


#include "HexLibrary.h"
#include "Utils/HexMetrics.h"

FVector UHexLibrary::HexCoordinatesFromPosition(FVector position)
{
	FHexCoordinates coordinates = FHexCoordinates::FromPosition(position);
	return FVector(coordinates.X, coordinates.Y, coordinates.Z);
}
