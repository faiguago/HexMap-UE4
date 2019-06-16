// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FEdgeVertices
{
	FVector v1, v2, v3, v4;

	FEdgeVertices() { }

	FEdgeVertices(FVector corner1, FVector corner2);

	static FEdgeVertices TerraceLerp(const FEdgeVertices& a, const FEdgeVertices& b, int32 step);
};

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

namespace EHexEdge
{
	enum Type
	{
		Flat,
		Slope,
		Cliff
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

	HexMetrics() {}
	~HexMetrics() {}
	
	static const int32 chunkSize;
	
	static const float hexScale;

	static const float outerRadius;

	static const float innerRadius;

	static const float solidFactor;

	static const float blendFactor;

	static const float elevationStep;

	static const float cellPerturbStrength;

	static const float elevationPerturbStrength;

	static const float horizontalTerraceStepSize;

	static const float verticalTerraceStepSize;

	static const int32 terracesPerSlop;

	static const int32 terraceSteps;

	static const float waterElevationOffset;

	static FVector GetFirstCorner(EHexDirection::Type direction);

	static FVector GetSecondCorner(EHexDirection::Type direction);

	static FVector GetFirstSolidCorner(EHexDirection::Type direction);

	static FVector GetSecondSolidCorner(EHexDirection::Type direction);

	static FVector GetBridge(EHexDirection::Type direction);

	static FVector TerraceLerp(const FVector& a, const FVector& b, int32 step);

	static FLinearColor TerraceColorLerp(const FLinearColor& a, const FLinearColor& b, int32 step);

	static EHexEdge::Type GetEdgeType(int32 elevation1, int32 elevation2);
	
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