// Fill out your copyright notice in the Description page of Project Settings.


#include "HexMetrics.h"

const int32 HexMetrics::chunkSize = 5;

const float HexMetrics::hexScale = 10.0f;

const float HexMetrics::outerRadius = 10.0f;

const float HexMetrics::solidFactor = .8f;

const float HexMetrics::blendFactor = 1.f - HexMetrics::solidFactor;

const float HexMetrics::elevationStep = 5.f * HexMetrics::hexScale;

const float HexMetrics::innerRadius = HexMetrics::outerRadius * 0.866025404f;

const int32 HexMetrics::terracesPerSlop = 3;

const int32 HexMetrics::terraceSteps = HexMetrics::terracesPerSlop * 2 + 1;

const float HexMetrics::waterElevationOffset = -0.5f;

const float HexMetrics::cellPerturbStrength = 10.f;

const float HexMetrics::elevationPerturbStrength = 15.f;

const float HexMetrics::horizontalTerraceStepSize = 1.f / HexMetrics::terraceSteps;

const float HexMetrics::verticalTerraceStepSize = 1.f / (HexMetrics::terracesPerSlop + 1);

static const FVector corners_datas[] = { 
	FVector(HexMetrics::outerRadius, 0.0f, 0.0f),
	FVector(0.5f * HexMetrics::outerRadius, HexMetrics::innerRadius, 0.0f),
	FVector(-0.5f * HexMetrics::outerRadius, HexMetrics::innerRadius, 0.0f),
	FVector(-HexMetrics::outerRadius, 0.0f, 0.0f),
	FVector(-0.5f * HexMetrics::outerRadius, -HexMetrics::innerRadius, 0.0f),
	FVector(0.5f * HexMetrics::outerRadius, -HexMetrics::innerRadius, 0.0f),
	FVector(HexMetrics::outerRadius, 0.0f, 0.0f)
};

const FVector* HexMetrics::corners = corners_datas;

EHexDirection::Type HexDirection::Opposite(EHexDirection::Type direction)
{
	return (EHexDirection::Type)(direction < 3 ? (direction + 3) : (direction - 3));
}

EHexDirection::Type HexDirection::Previous(EHexDirection::Type direction)
{
	return (EHexDirection::Type)(direction == EHexDirection::NW ? EHexDirection::NE : (direction + 1));
}

EHexDirection::Type HexDirection::Next(EHexDirection::Type direction)
{
	return (EHexDirection::Type)(direction == EHexDirection::NE ? EHexDirection::NW : (direction - 1));
}

FVector HexMetrics::GetFirstCorner(EHexDirection::Type direction)
{
	return corners[direction + 1] * hexScale;
}

FVector HexMetrics::GetSecondCorner(EHexDirection::Type direction)
{
	return corners[direction] * hexScale;
}

FVector HexMetrics::GetFirstSolidCorner(EHexDirection::Type direction)
{
	return corners[direction + 1] * solidFactor * hexScale;
}

FVector HexMetrics::GetSecondSolidCorner(EHexDirection::Type direction)
{
	return corners[direction] * solidFactor * hexScale;
}

FVector HexMetrics::GetBridge(EHexDirection::Type direction)
{
	return (corners[direction] + corners[direction + 1]) * blendFactor * hexScale;
}

FVector HexMetrics::TerraceLerp(const FVector& a, const FVector& b, int32 step)
{
	FVector result = a;
	float h = step * HexMetrics::horizontalTerraceStepSize;
	result.Y += (b.Y - a.Y) * h;
	result.X += (b.X - a.X) * h;
	float v = ((step + 1) / 2) * HexMetrics::verticalTerraceStepSize;
	result.Z += (b.Z - a.Z) * v;
	return result;
}

FLinearColor HexMetrics::TerraceColorLerp(const FLinearColor& a, const FLinearColor& b, int32 step)
{
	float h = step * HexMetrics::horizontalTerraceStepSize;
	return FMath::Lerp(a, b, h);
}

EHexEdge::Type HexMetrics::GetEdgeType(int32 elevation1, int32 elevation2)
{
	if (elevation1 == elevation2)
		return EHexEdge::Flat;

	int32 delta = elevation2 - elevation1;
	if (delta == 1 || delta == -1)
		return EHexEdge::Slope;

	return EHexEdge::Cliff;
}

FEdgeVertices::FEdgeVertices(FVector corner1, FVector corner2)
{
	v1 = corner1;
	v2 = FMath::Lerp(corner1, corner2, 1.f / 3.f);
	v3 = FMath::Lerp(corner1, corner2, 2.f / 3.f);
	v4 = corner2;
}

FEdgeVertices FEdgeVertices::TerraceLerp(const FEdgeVertices& a, const FEdgeVertices& b, int32 step)
{
	FEdgeVertices result;

	result.v1 = HexMetrics::TerraceLerp(a.v1, b.v1, step);
	result.v2 = HexMetrics::TerraceLerp(a.v2, b.v2, step);
	result.v3 = HexMetrics::TerraceLerp(a.v3, b.v3, step);
	result.v4 = HexMetrics::TerraceLerp(a.v4, b.v4, step);

	return result;
}
