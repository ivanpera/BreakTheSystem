// Fill out your copyright notice in the Description page of Project Settings.


#include "Chain.h"
#include "Block.h"
#include "Grid.h"

Chain::Chain()
{
}

Chain::~Chain()
{
}

Chain const Chain::Rotate() const
{
    Chain Rotated(*this);
    Rotated.RRotate(Rotated.Root);
    return Rotated;
}

void Chain::RRotate(FChainItem& ChainItem)
{
    ChainItem.Direction = GetNextDirection(ChainItem.Direction);
    for (auto& Child : ChainItem.Children)
    {
        RRotate(Child);
    }
}

Chain const* Chain::GenerateChain(FVector2D const& SpawnArea, int32 MaxNumBlocks)
{
    TSet<FString> OccupiedPositions;
    Chain* c = new Chain();
    if (MaxNumBlocks)
    {
        uint8 SpawnX = SpawnArea.X * 0.5f;
        uint8 SpawnY = SpawnArea.Y;
        //Set at bottom centre of spawnarea
        c->Root.BlockPosition = FVector2D(SpawnX, SpawnY);
        c->Root.Direction = EDirection::NO_DIRECTION;
        c->Root.Parent = nullptr;
        OccupiedPositions.Add(FString::Printf(TEXT("%d%d"), SpawnX, SpawnY));
        c->GenerateChildren(c->Root, SpawnArea, OccupiedPositions, --MaxNumBlocks);
    }
    return c;
}

TMap<EDirection, FVector2D> const Chain::GetValidChildrenPositions(FChainItem const& Node, FVector2D const& SpawnArea, TSet<FString>& OccupiedPositions) const
{
    FVector2D const& BlockPos = Node.BlockPosition;
    TMap<EDirection, FVector2D> TargetPos;
    TargetPos.Add(EDirection::LEFT, FVector2D(BlockPos.X - 1, BlockPos.Y));
    TargetPos.Add(EDirection::RIGHT, FVector2D(BlockPos.X + 1, BlockPos.Y));
    TargetPos.Add(EDirection::UP, FVector2D(BlockPos.X, BlockPos.Y - 1));
    TargetPos.Add(EDirection::DOWN, FVector2D(BlockPos.X, BlockPos.Y + 1));
    TMap<EDirection, FVector2D> ValidPos;
    for (auto const& TestPos : TargetPos)
    {
        FString TestPosString = FString::Printf(TEXT("%d%d"), (uint8)TestPos.Value.X, (uint8)TestPos.Value.Y);
        if (
            TestPos.Value.Y <= SpawnArea.Y &&
            TestPos.Value.X <= SpawnArea.X &&
            !OccupiedPositions.Contains(TestPosString)
            )
        {
            ValidPos.Add(TestPos);
            OccupiedPositions.Add(TestPosString);
        }
    }
    return ValidPos;
}

void Chain::GenerateChildren(FChainItem& Node, FVector2D const& SpawnArea, TSet<FString>& OccupiedPositions, int32& MaxNumBlocks, float const SpawnProbabilityModifier)
{
    TMap<EDirection, FVector2D> Positions = GetValidChildrenPositions(Node, SpawnArea, OccupiedPositions);
    if (!Positions.Num() || !MaxNumBlocks)
    {
        return;
    }
    float SpawnProbIncrement = 1.f / Positions.Num();
    float SpawnProbDecrement = 1.f / (Positions.Num() + 1);
    float SpawnProb = (1.f / Positions.Num())* SpawnProbabilityModifier;
    for (auto const& Pos : Positions)
    {
        if (FMath::FRand() < SpawnProb)
        {
            //Direction, Children, Parent, BlockPosition
            Node.Children.Add(FChainItem(Pos.Key, Node, Pos.Value));
            SpawnProb -= SpawnProbDecrement;
            if (--MaxNumBlocks <= 0)
            {
                return;
            }
        }
        else
        {
            SpawnProb += SpawnProbIncrement;
        }
    }
    for (auto& Child : Node.Children)
    {
        GenerateChildren(Child, SpawnArea, OccupiedPositions, MaxNumBlocks, SpawnProbabilityModifier - .25f);
    }
}

FVector2D const Chain::GetVectorFromDirection(EDirection const& Direction) const
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

EDirection const Chain::GetNextDirection(EDirection const& Direction) const
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

TArray<FVector2D> const Chain::GetBlocksPositions(FVector2D const& RootPosition) const
{
    return RGetBlocksPositions(Root, RootPosition);
}

TArray<FVector2D> const Chain::RGetBlocksPositions(FChainItem const& Node, FVector2D const& RootPosition) const
{
    TArray<FVector2D> Result;
    FVector2D ResultPos = RootPosition + GetVectorFromDirection(Node.Direction);
    Result.Add(ResultPos);
    for (auto const& Child : Node.Children)
    {
        Result.Append(RGetBlocksPositions(Child, ResultPos));
    }
    return Result;
}

