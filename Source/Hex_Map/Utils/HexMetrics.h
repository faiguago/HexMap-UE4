// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

namespace EHexDirection
{
	enum Type
	{
		NE,
		E,
		SE,
		SW,
		W,
		NW
	};
}

class HexDirection
{
public:

	static EHexDirection::Type Opposite(EHexDirection::Type direction);

	static EHexDirection::Type Previous(EHexDirection::Type direction);

	static EHexDirection::Type Next(EHexDirection::Type direction);
};


class HEX_MAP_API HexMetrics
{
public:
	HexMetrics();
	~HexMetrics();

	static const float hexScale;

	static const float outerRadius;

	static const float innerRadius;

	static const float solidFactor;

	static const float blendFactor;

	static const float elevationStep;

	static FVector GetFirstCorner(EHexDirection::Type direction);

	static FVector GetSecondCorner(EHexDirection::Type direction);

	static FVector GetFirstSolidCorner(EHexDirection::Type direction);

	static FVector GetSecondSolidCorner(EHexDirection::Type direction);

	static FVector GetBridge(EHexDirection::Type direction);

private:

	static const FVector* corners;
};


struct FHexCoordinates
{

	int32 X;

	int32 Y;

	int32 Z;

	FHexCoordinates() { }

	FHexCoordinates(int32 x, int32 z)
	{
		X = x;
		Z = z;
		Y = -x - z;
	}

	FORCEINLINE static FHexCoordinates FromOffsetCoordinates(const int32& x, const int32& z);

	FString ToString() const;

	FString GetName() const;

	FORCEINLINE static FHexCoordinates FromPosition(const FVector& position);
};

FORCEINLINE FHexCoordinates FHexCoordinates::FromOffsetCoordinates(const int32& x, const int32& z)
{
	return FHexCoordinates(x - z / 2, z);
}

FORCEINLINE FString FHexCoordinates::ToString() const
{
	return FString::Printf(TEXT("%d\n%d\n%d"), X, Y, Z);
}

FORCEINLINE FString FHexCoordinates::GetName() const
{
	return FString::Printf(TEXT("%d, %d, %d"), X, Y, Z);
}

FORCEINLINE FHexCoordinates FHexCoordinates::FromPosition(const FVector& position)
{
	float x = position.Y / (HexMetrics::innerRadius * 2.f * HexMetrics::hexScale);
	float y = -x;

	float offset = position.X / (HexMetrics::outerRadius * 3.f * HexMetrics::hexScale);
	x -= offset;
	y -= offset;

	int32 iX = FMath::RoundToInt(x);
	int32 iY = FMath::RoundToInt(y);
	int32 iZ = FMath::RoundToInt(-x - y);

	if (iX + iY + iZ != 0)
	{
		float dX = FMath::Abs(x - iX);
		float dY = FMath::Abs(y - iY);
		float dZ = FMath::Abs(-x - y - iZ);

		if (dX > dY && dX > dZ)
			iX = -iY - iZ;
		else
			iZ = -iX - iY;
	}

	return FHexCoordinates(iX, iZ);
}