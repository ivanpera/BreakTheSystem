// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "Block.h"
#include "Chain.h"
#include "ChainItem.h"

static float constexpr MAX_CHAIN_SIZE = 4;
static FGridSpace const DEFAULT_SPACE(false, EBlockState::NO_STATE, "");
// Sets default values
UGrid::UGrid()
{}

// Called when the game starts or when spawned
void UGrid::Initialize(uint8 _NumRows, uint8 _NumCols, FVector const& Centre, FVector const& Extents)
{
	NumRows = _NumRows;
	NumCols = _NumCols;
	Width = Extents.Y * 2.0f;
	Height = Extents.Z * 2.0f;
	float const DividerX = GetNumCols() <= 0 ? 1.0f : GetNumCols() * 1.0f;
	float const DividerY = GetNumRows() <= 0 ? 1.0f : GetNumRows() * 1.0f;
	Grid.Init(DEFAULT_SPACE, GetNumCols() * GetNumRows());
	//PreviewGrid.Init(DEFAULT_SPACE, MAX_CHAIN_SIZE * MAX_CHAIN_SIZE);
	CurrentChainBlocks.Init(nullptr, MAX_CHAIN_SIZE);
	StepX = Width / DividerX;
	StepY = Height / DividerY;
	Origin = Centre
		+ (FVector(0.0f, -Extents.Y, Extents.Z)
			+ FVector(0.0f, StepX * 0.5f, -StepY * 0.5f));
	//ChainQueue->Initialize(MAX_CHAIN_SIZE, MAX_CHAIN_SIZE, MAX_CHAIN_SIZE);
}

bool UGrid::CanMoveBlock(ABlock* Block, FVector2D const& Movement, uint8 const& IgnoreStates)
{
	FVector2D BlockPos = Block->GetPosition();
	FVector2D TargetPos = BlockPos + Movement;
	if (IsInBounds(BlockPos) && !IsInBounds(TargetPos))
	{
		return false;
	}
	FGridSpace const& ElementAt = GetElementAt(TargetPos.X, TargetPos.Y);
	if (!ElementAt.bOccupied || ((uint8)ElementAt.BlockState & IgnoreStates))// || CanMoveBlock(ElementAt, Movement, StatesMask))
	{
		return true;
	}
	return false;
}

void UGrid::OnRotate()
{
	if (CurrentChain)
	{
		bRotate = true;
	}
}

void UGrid::OnForceDown()
{
	if (CurrentChain)
	{
		bForceDown = true;
	}
}

void UGrid::OnMove(FVector2D const& Movement)
{
	if (CurrentChain)
	{
		bMove = true;
		MovementInput = Movement;
	}
}

void UGrid::RotateChain()
{
	if (CurrentChain)
	{
		UChain const* Rotated = CurrentChain->Rotate();
		TArray<FVector2D> NewPositions = Rotated->GetBlocksPositions(CurrentChainBlocks[0]->GetPosition());
		FVector2D Offset(0.f, 0.f);
		for (FVector2D const& NewPos : NewPositions)
		{
			if (NewPos.Y < 0)
			{
				Offset.Y = FMath::Max(FMath::Abs(Offset.Y), FMath::Abs(NewPos.Y));
			}
			else if (NewPos.Y >= NumCols)
			{
				Offset.Y = -FMath::Max(FMath::Abs(Offset.Y), FMath::Abs(NumCols - (NewPos.Y + 1)));
			}
			if (NewPos.X < 0)
			{
				Offset.X = FMath::Max(FMath::Abs(Offset.X), FMath::Abs(NewPos.X));
			}
			else if (NewPos.X >= NumRows)
			{
				Offset.X = -FMath::Max(FMath::Abs(Offset.X), FMath::Abs(NumRows - (NewPos.X + 1)));
			}
		}
		for (FVector2D const& NewPos : NewPositions)
		{
			FVector2D const NewOffsetPos(NewPos + Offset);
			
			FGridSpace const& ElementAt = GetElementAt(NewOffsetPos.X, NewOffsetPos.Y);

			if (ElementAt.bOccupied && ElementAt.BlockState != EBlockState::IN_CHAIN)
			{
				return;
			}
		}
		int32 ChainBlocks = CurrentChainBlocks.Num();
		for (int32 i = 0; i < ChainBlocks; i++)
		{
			ABlock* CurrBlock = CurrentChainBlocks[i];
			FVector2D CurrBlockPos = CurrBlock->GetPosition();
			FVector2D TargetPos = NewPositions[i] + Offset;
			FGridSpace const& ElementFrom = GetElementAt(CurrBlockPos.X, CurrBlockPos.Y);
			if (ElementFrom.BlockId == CurrBlock->GetName())
			{
				SetElementAt(CurrBlockPos.X, CurrBlockPos.Y, {});
			}
			SetElementAt(TargetPos.X, TargetPos.Y, CurrBlock);
			CurrBlock->SetPosition(TargetPos);
		}
		CurrentChain = Rotated;
	}
}

void UGrid::MoveBlock(ABlock* Block, FVector2D const& Movement)
{
	FVector2D BlockPos = Block->GetPosition();
	FVector2D TargetPos = BlockPos + Movement;
	FGridSpace const& ElementFrom = GetElementAt(BlockPos.X, BlockPos.Y);
	if (ElementFrom.BlockId == Block->GetName())
	{
		SetElementAt(BlockPos.X, BlockPos.Y, nullptr);
	}
	SetElementAt(TargetPos.X, TargetPos.Y, Block);
	Block->SetPosition(TargetPos);
}

bool UGrid::CanMoveChain(FVector2D const& Movement)
{
	for (ABlock* Block : CurrentChainBlocks)
	{
		if (!CanMoveBlock(Block, Movement, (uint8)EBlockState::IN_CHAIN))
		{
			return false;
		}
	}
	return true;
}

void UGrid::DeleteCurrentChain()
{
	if (CurrentChain)
	{
		for (ABlock* Block : CurrentChainBlocks)
		{
			Block->State = EBlockState::FALLING;
			//TODO: Move to another function
			FVector2D const& BlockPosition = Block->GetPosition();
			if (BlockPosition.Y < 0 && CanMoveBlock(Block, { BlockPosition.X, 0 }, (uint8)EBlockState::FALLING | (uint8)EBlockState::IN_CHAIN))
			{
				//Set bLost
				//Block->SetPosition({ BlockLocation.X, 0.0f });
			}
			SetElementAt(BlockPosition.X, BlockPosition.Y, Block);
		}
		bRotate = false;
		bMove = false;
		MovementInput = { .0f, .0f };
		bForceDown = false;
		CurrentChain = nullptr;
		CurrentChainBlocks.Reset(MAX_CHAIN_SIZE);
		//bHandleInput = false;
		//bUpdateGravity = true;
	}
}

void UGrid::MoveChain(FVector2D const& Movement)
{
	for (ABlock* Block : CurrentChainBlocks)
	{
		MoveBlock(Block, Movement);
	}
}

void UGrid::FallChainBlocks(float DeltaTime)
{
	if (UpdateTimeLeft <= 0 || bForceDown)
	{
		if (CurrentChain)
		{
			if (!CanMoveChain({ 0.0f, 1.0f }))
			{
				DeleteCurrentChain();
			}
			else
			{
				MoveChain({ 0.0f, 1.0f });
			}
			UpdateTimeLeft = UpdateTimeSeconds;
			bForceDown = false;
		}
		else if (OnChainStop.IsBound())
		{
			OnChainStop.Broadcast();
		}
	}
	else
	{
		UpdateTimeLeft -= DeltaTime;
	}
}

void UGrid::HandlePlayerMovement()
{
	if (CurrentChain)
	{
		if (bRotate)
		{
			RotateChain();
			bRotate = false;
		}
		if (bMove)
		{
			if(CanMoveChain(MovementInput))
			{
				MoveChain(MovementInput);
			}
			MovementInput = { .0f, .0f };
			bMove = false;
		}
	}
}

void UGrid::UpdateGrid(float DeltaTime)
{
	for (ABlock* Block : GridBlocks)
	{
		if (Block->State == EBlockState::IN_CHAIN || Block->State == EBlockState::PENDING_RESOLUTION) {
			continue;
		}
		FVector2D Movement(0.f, 1.f);
		if (CanMoveBlock(Block, Movement, 0))
		{
			MoveBlock(Block, Movement);
			Block->State = EBlockState::FALLING;
		}
		else
		{
			Block->State = EBlockState::IDLE;
		}
	}
	//bUpdateGravity = false;
	FallChainBlocks(DeltaTime);
	HandlePlayerMovement();

	//Resolve blocks
}

uint8 UGrid::GetNumCols() const
{
	return NumCols;
}

uint8 UGrid::GetNumRows() const
{
	return NumRows;
}

float UGrid::GetWidth() const
{
	return Width;
}

float UGrid::GetHeight() const
{
	return Height;
}

float UGrid::GetStepX() const
{
	return StepX;
}

float UGrid::GetStepY() const
{
	return StepY;
}

bool UGrid::IsInBounds(FVector2D const& TestPosition) const
{
	return TestPosition.X >= 0 &&
		TestPosition.X < GetNumCols() &&
		TestPosition.Y >= 0 &&
		TestPosition.Y < GetNumRows();
}

void UGrid::SpawnChain(UChain const* Chain)
{
	CurrentChain = Chain;
	CurrentChainBlocks.Reset(MAX_CHAIN_SIZE);
	float SpawnX = FMath::FloorToFloat(NumCols * .5f);
	TArray<FVector2D> ChainBlocksPositions = CurrentChain->GetBlocksPositions({SpawnX, -1});
	TArray<UChainItem const*> ChainItems = CurrentChain->GetItems();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform Transform;
	for (int i = 0; i < ChainBlocksPositions.Num(); i++)
	{
		FVector2D const& BlockPos = ChainBlocksPositions[i];
		UChainItem const* Item = ChainItems[i];
		Transform.SetLocation(Origin + FVector(0.0f, BlockPos.X * GetStepX(), -BlockPos.Y * GetStepY()));
		auto NewItem = GetWorld()->SpawnActor<ABlock>(Item->BlockType, Transform, SpawnParams);
		float NewItemExtents = NewItem->GetExtents();
		NewItem->State = EBlockState::IN_CHAIN;
		float MinDimension = FMath::Min(
			(GetWidth() / (NewItemExtents * GetNumCols())) * .5f,
			(GetHeight() / (NewItemExtents * GetNumRows())) * .5f
		);
		CurrentChainBlocks.Add(NewItem);
		GridBlocks.Add(NewItem);
		SetElementAt(BlockPos.X, BlockPos.Y, NewItem);
		NewItem->SetScale(FVector(MinDimension, MinDimension, MinDimension));
		NewItem->Initialize(this, BlockPos);
	}
	//bHandleInput = true;
}

FGridSpace const& UGrid::GetElementAt(uint8 X, uint8 Y) const
{
	if (X >= 0 &&
		X < GetNumCols() &&
		Y >= 0 &&
		Y < GetNumRows())
	{
		return Grid[X + Y * NumCols];
	}
	return DEFAULT_SPACE;
}

void UGrid::SetElementAt(uint8 X, uint8 Y, ABlock const* NewElement)
{
	if (X >= 0 &&
		X < GetNumCols() &&
		Y >= 0 &&
		Y < GetNumRows())
	{
		if (IsValid(NewElement))
		{
			Grid[X + Y * NumCols].bOccupied = true;
			Grid[X + Y * NumCols].BlockState = NewElement->State;
			Grid[X + Y * NumCols].BlockId = NewElement->GetName();
			Grid[X + Y * NumCols].BlockType = NewElement->StaticClass();
		}
		else
		{
			Grid[X + Y * NumCols].bOccupied = false;
			Grid[X + Y * NumCols].BlockState = EBlockState::NO_STATE;
			Grid[X + Y * NumCols].BlockId = "";
			Grid[X + Y * NumCols].BlockType = nullptr;
		}
	}
}

FVector UGrid::GetOrigin()  const
{
	return Origin;
}
