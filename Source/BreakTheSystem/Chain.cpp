// Fill out your copyright notice in the Description page of Project Settings.


#include "Chain.h"
#include "Block.h"
#include "Grid.h"
#include "Chainitem.h"

UChain::UChain()
{
}

UChain const* UChain::Rotate() const
{
    UChain* Rotated = DuplicateObject<UChain>(this, this->GetOuter());
    Rotated->RRotate(Rotated->Root);
    return Rotated;
}

void UChain::RRotate(UChainItem* ChainItem)
{
    ChainItem->Direction = GetNextDirection(ChainItem->Direction);
    for (auto Child : ChainItem->Children)
    {
        RRotate(Child);
    }
}

FVector2D const UChain::GetVectorFromDirection(EDirection const& Direction) const
{
    switch (Direction)
    {
    case EDirection::LEFT:
            return { -1.0f, 0.0f };
        break;
    case EDirection::RIGHT:
            return { 1.0f, 0.0f };
        break;
    case EDirection::UP:
            return { 0.0f, -1.0f };
        break;
    case EDirection::DOWN:
            return { 0.0f, 1.0f };
        break;
    default:
        return { 0.0f, 0.0f };
    }
}

EDirection const UChain::GetNextDirection(EDirection const& Direction) const
{
    switch (Direction)
    {
    case EDirection::LEFT:
        return EDirection::DOWN;
        break;
    case EDirection::RIGHT:
        return EDirection::UP;
        break;
    case EDirection::UP:
        return EDirection::LEFT;
        break;
    case EDirection::DOWN:
        return EDirection::RIGHT;
        break;
    default:
        return EDirection::NO_DIRECTION;
    }
}

TArray<FVector2D> const UChain::GetBlocksPositions(FVector2D const& RootPosition) const
{
    return RGetBlocksPositions(Root, RootPosition);
}

TArray<FVector2D> const UChain::RGetBlocksPositions(UChainItem const* Node, FVector2D const& RootPosition) const
{
    TArray<FVector2D> Result;
    FVector2D ResultPos = RootPosition + GetVectorFromDirection(Node->Direction);
    Result.Add(ResultPos);
    for (UChainItem const* Child : Node->Children)
    {
        Result.Append(RGetBlocksPositions(Child, ResultPos));
    }
    return Result;
}

TArray<UChainItem const*> const UChain::GetItems() const
{
    return RGetItems(Root);
}

TArray<UChainItem const*> const UChain::RGetItems(UChainItem const* Node) const
{
    TArray<UChainItem const*> Result;
    Result.Add(Node);
    for (UChainItem const* Child : Node->Children)
    {
        Result.Append(RGetItems(Child));
    }
    return Result;
}


