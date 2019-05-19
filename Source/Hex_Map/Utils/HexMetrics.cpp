// Fill out your copyright notice in the Description page of Project Settings.


#include "HexMetrics.h"

const float HexMetrics::hexScale = 10.0f;

const float HexMetrics::outerRadius = 10.0f;

const float HexMetrics::solidFactor = .75f;

const float HexMetrics::blendFactor = 1.f - HexMetrics::solidFactor;

const float HexMetrics::elevationStep = 5.f * HexMetrics::hexScale;

const float HexMetrics::innerRadius = HexMetrics::outerRadius * 0.866025404f;

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

HexMetrics::HexMetrics()
{
}

HexMetrics::~HexMetrics()
{
}

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