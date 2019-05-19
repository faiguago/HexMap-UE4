// Fill out your copyright notice in the Description page of Project Settings.


#include "HexCell.h"
#include "Components/TextRenderComponent.h"

// Sets default values
AHexCell::AHexCell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComp");

	textComponent = CreateDefaultSubobject<UTextRenderComponent>("TextComp");
	textComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHexCell::BeginPlay()
{
	Super::BeginPlay();

	orTextElevation = textComponent->GetComponentTransform().GetLocation().Z;
}

float AHexCell::GetElevation()
{
	return elevation;
}

void AHexCell::SetElevation(const float& elevation)
{
	this->elevation = elevation;
	
	FVector position = GetActorLocation();
	position.Z = elevation * HexMetrics::elevationStep;
	SetActorLocation(position);

	FVector textPos = textComponent->GetComponentTransform().GetLocation();
	textPos.Z = orTextElevation + elevation * HexMetrics::elevationStep;
	textComponent->SetWorldLocation(textPos);
}

void AHexCell::UpdateText(const FString& newText)
{
	textComponent->SetText(newText);
}

AHexCell* AHexCell::GetNeighbor(EHexDirection::Type direction)
{
	return neighbors[(int32)direction];
}

void AHexCell::SetNeighbor(EHexDirection::Type direction, AHexCell* cell)
{
	neighbors[direction] = cell;
	EHexDirection::Type opposite = HexDirection::Opposite(direction);
	cell->neighbors[opposite] = this;
}

void AHexCell::Initialize()
{
	neighbors.SetNum(6);
}
