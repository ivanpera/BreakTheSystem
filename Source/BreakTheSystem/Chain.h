// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chain.generated.h"

class UChainItem;
enum class EDirection : uint8;
/**
 * 
 */
UCLASS()
class BREAKTHESYSTEM_API UChain : public UObject
{
	GENERATED_BODY()
public:
	UChain();
	//~Chain();

	//static Chain const* GenerateChain(FVector2D const& SizeLimits, int32 MaxNumBlocks);
	UChain const* Rotate() const;
	TArray<FVector2D> const GetBlocksPositions(FVector2D const& RootPosition) const;
	TArray<UChainItem const*> const GetItems() const;
private:
	friend class UChainGenerator;
	UPROPERTY()
	UChainItem* Root;
	TArray<FVector2D> const RGetBlocksPositions(UChainItem const* Node, FVector2D const& RootPosition) const;
	TArray<UChainItem const*> const RGetItems(UChainItem const* Node) const;

	FVector2D const GetVectorFromDirection(EDirection const& Direction) const;
	EDirection const GetNextDirection(EDirection const& Direction) const;
	void RRotate(UChainItem* ChainItem);
	//void GenerateChildren(UChainItem* Node, FVector2D const& SizeLimits, TSet<FString>& OccupiedPositions, int32& MaxNumBlocks,float const SpawnProbabilityModifier = 1.0f);
};
