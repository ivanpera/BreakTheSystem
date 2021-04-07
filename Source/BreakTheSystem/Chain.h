// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class EDirection
{
	NO_DIRECTION,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct FChainItem
{
	FChainItem() = default;
	FChainItem(EDirection const& Direction, FChainItem& Parent, FVector2D const& BlockPosition) :
		Direction(Direction),
		Parent(&Parent),
		BlockPosition(BlockPosition) { }
	EDirection Direction;
	TArray<FChainItem> Children;
	FChainItem* Parent;
private:
	friend class Chain;
	FVector2D BlockPosition; //Only used in chain generation
};


/**
 * 
 */
class BREAKTHESYSTEM_API Chain
{
public:
	Chain();
	~Chain();

	static Chain const* GenerateChain(FVector2D const& SizeLimits, int32 MaxNumBlocks);
	Chain const Rotate() const;
	TArray<FVector2D> const GetBlocksPositions(FVector2D const& RootPosition) const;
private:
	FChainItem Root;
	TArray<FVector2D> const RGetBlocksPositions(FChainItem const& Node, FVector2D const& RootPosition) const;
	TMap<EDirection, FVector2D> const GetValidChildrenPositions(FChainItem const& Node, FVector2D const& SizeLimits, TSet<FString>& OccupiedPositions) const;
	FVector2D const GetVectorFromDirection(EDirection const& Direction) const;
	EDirection const GetNextDirection(EDirection const& Direction) const;
	void RRotate(FChainItem& ChainItem);
	void GenerateChildren(FChainItem& Node, FVector2D const& SizeLimits, TSet<FString>& OccupiedPositions, int32& MaxNumBlocks,float const SpawnProbabilityModifier = 1.0f);
};
