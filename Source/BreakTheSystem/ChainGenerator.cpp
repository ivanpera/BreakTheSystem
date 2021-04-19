// Fill out your copyright notice in the Description page of Project Settings.


#include "ChainGenerator.h"
#include "Grid.h"
#include "Block.h"
#include "Chain.h"
#include "ChainItem.h"

#if UE_BUILD_DEBUG
uint32 GetTypeHash(const FWeightedBlock& Thing)
{
	uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FWeightedBlock));
	return Hash;
}
#endif

bool FWeightedBlock::Equals(const FWeightedBlock& Other) const
{
    return BlockType.Get()->GetName() == Other.BlockType.Get()->GetName();
}

void UChainGenerator::PostInitProperties()
{
	Super::PostInitProperties();
    for (FWeightedBlock& wb : WeightedBlocks)
    {
        TotalWeight += wb.Weight;
    }
}

TSubclassOf<ABlock> UChainGenerator::GetRandomBlockType()
{
    if (WeightedBlocks.Num())
    {
        float Rand = 0.f;
        while (Rand == 0.f)
        {
            Rand = FMath::FRand() * TotalWeight;
        }
        for (FWeightedBlock const& WeightedBlock : WeightedBlocks)
        {
            Rand -= WeightedBlock.Weight;
            if (Rand <= 0.f)
            {
                return WeightedBlock.BlockType;
            }
        }
        UE_LOG(LogTemp, Warning, TEXT("UChainGenerator::GetRandomBlockType: this message should not appear"));
        return ABlock::StaticClass();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No block types in the set, returning base ABlock class"));
        return ABlock::StaticClass();
    }
}

//void UChainGenerator::SpawnChainBlocks(TArray<FVector2D> const& BlocksPositions)
//{
//	FActorSpawnParameters SpawnParams;
//	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//	FTransform Transform;
//	//if (WeightedBlocks.Num())
//	//{
//	//	AGrid* ParentGrid = Cast<AGrid>(GetOwner());
//	//	if (ParentGrid)
//	//	{
//	//		for (auto const& BlockPos : BlocksPositions)
//	//		{
//	//			//If the position is blocked, it's game over
//	//			Transform.SetLocation(ParentGrid->GetOrigin() + FVector(0.0f, BlockPos.X * ParentGrid->GetStepX(), -BlockPos.Y * ParentGrid->GetStepY()));
//	//			auto NewItem = GetWorld()->SpawnActor<ABlock>(WeightedBlocks.begin()->BlockType, Transform, SpawnParams);
//	//			float NewItemExtents = NewItem->GetExtents();
//	//			NewItem->State = EBlockState::IN_CHAIN;
//	//			float MinDimension = FMath::Min(
//	//				(ParentGrid->GetWidth()/ (NewItemExtents * ParentGrid->GetNumCols())) * .5f,
//	//				(ParentGrid->GetHeight()/ (NewItemExtents * ParentGrid->GetNumRows())) * .5f
//	//			);
//	//			NewItem->SetActorScale3D(FVector(1.0f, MinDimension, MinDimension));
//	//			NewItem->Initialize(ParentGrid, BlockPos);
//	//		}
//	//	}
//	//}
//	//else
//	//{
//	//	UE_LOG(LogTemp, Warning, TEXT("Missing BlockType"));
//	//}
//}

UChain* UChainGenerator::GenerateChain(FVector2D const& SpawnArea, int32 MaxNumBlocks)
{
    TSet<FString> OccupiedPositions;
    UChain* c = NewObject<UChain>();
    if (MaxNumBlocks)
    {
        uint8 SpawnX = SpawnArea.X * 0.5f;
        uint8 SpawnY = SpawnArea.Y;
        //Set at bottom centre of spawnarea
        c->Root = NewObject<UChainItem>();
        c->Root->Initialize(EDirection::NO_DIRECTION, nullptr, { SpawnX * 1.0f, SpawnY * 1.0f }, GetRandomBlockType());
        OccupiedPositions.Add(FString::Printf(TEXT("%d%d"), SpawnX, SpawnY));
        GenerateChildren(c->Root, SpawnArea, OccupiedPositions, --MaxNumBlocks);
    }
    return c;
}

void UChainGenerator::GenerateChildren(UChainItem* Node, FVector2D const& SpawnArea, TSet<FString>& OccupiedPositions, int32& MaxNumBlocks, float const SpawnProbabilityModifier)
{
    TMap<EDirection, FVector2D> Positions = GetValidChildrenPositions(Node, SpawnArea, OccupiedPositions);
    if (!Positions.Num() || !MaxNumBlocks)
    {
        return;
    }
    float SpawnProbIncrement = 1.f / Positions.Num();
    float SpawnProbDecrement = 1.f / (Positions.Num() + 1);
    float SpawnProb = (1.f / Positions.Num()) * SpawnProbabilityModifier;
    for (auto const& Pos : Positions)
    {
        if (FMath::FRand() < SpawnProb)
        {
            //Direction, Children, Parent, BlockPosition
            UChainItem* Child = NewObject<UChainItem>();
            Child->Initialize(Pos.Key, Node, Pos.Value, GetRandomBlockType());
            Node->Children.Add(Child);
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
    for (auto& Child : Node->Children)
    {
        GenerateChildren(Child, SpawnArea, OccupiedPositions, MaxNumBlocks, SpawnProbabilityModifier - .25f);
    }
}

TMap<EDirection, FVector2D> const UChainGenerator::GetValidChildrenPositions(UChainItem const* Node, FVector2D const& SpawnArea, TSet<FString>& OccupiedPositions) const
{
    FVector2D const& BlockPos = Node->BlockPosition;
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
