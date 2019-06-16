// Fill out your copyright notice in the Description page of Project Settings.


#include "HexLibrary.h"
#include "Utils/HexMetrics.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"

FVector UHexLibrary::HexCoordinatesFromPosition(FVector position)
{
	FHexCoordinates coordinates = FHexCoordinates::FromPosition(position);
	return FVector(coordinates.X, coordinates.Y, coordinates.Z);
}

FLinearColor UHexLibrary::GetBilinearPixel(UTexture2D* tex, float u, float v)
{
	FTexture2DMipMap* MipMap = &tex->PlatformData->Mips[0];
	const FColor* FormatedImageData = static_cast<const FColor*>(MipMap->BulkData.LockReadOnly());

	int32 x = FMath::FloorToInt(u);
	int32 y = FMath::FloorToInt(v);

	u = u - x;
	v = v - y;

	FLinearColor q11 = FormatedImageData[(y % MipMap->SizeY) * MipMap->SizeX + (x % MipMap->SizeX)].ReinterpretAsLinear();
	FLinearColor q12 = FormatedImageData[((y + 1) % MipMap->SizeY) * MipMap->SizeX + (x % MipMap->SizeX)].ReinterpretAsLinear();
	FLinearColor q21 = FormatedImageData[(y % MipMap->SizeY) * MipMap->SizeX + ((x + 1) % MipMap->SizeX)].ReinterpretAsLinear();
	FLinearColor q22 = FormatedImageData[((y + 1) % MipMap->SizeY) * MipMap->SizeX + ((x + 1) % MipMap->SizeX)].ReinterpretAsLinear();

	tex->PlatformData->Mips[0].BulkData.Unlock();

	FLinearColor xy1 = (1 - u) * q11 + u * q21;
	FLinearColor xy2 = (1 - u) * q12 + u * q22;
	
	FLinearColor result = (1 - v) * xy1 + v * xy2;
	
	return result;
}