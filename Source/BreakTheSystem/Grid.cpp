// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "Block.h"
#include "ResolvableBlock.h"
#include "Chain.h"
#include "ChainItem.h"

//TODO: move to chain generator
static float constexpr MAX_CHAIN_SIZE = 4;

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
	Grid.Init(nullptr, GetNumCols() * GetNumRows());
	CurrentChainBlocks.Init(nullptr, MAX_CHAIN_SIZE);
	StepX = Width / DividerX;
	StepY = Height / DividerY;
	Origin = Centre
		+ (FVector(0.0f, -Extents.Y, Extents.Z)
			+ FVector(0.0f, StepX * 0.5f, -StepY * 0.5f));
}

bool UGrid::CanMoveBlock(ABlock* Block, FVector2D const& Movement, uint8 const& IgnoreStates)
{
	FVector2D BlockPos = Block->GetPosition();
	FVector2D TargetPos = BlockPos + Movement;
	if (IsInBounds(BlockPos) && !IsInBounds(TargetPos))
	{
		return false;
	}
	ABlock* ElementAt = GetElementAt(TargetPos.X, TargetPos.Y);
	if (!ElementAt || ((uint8)ElementAt->State & IgnoreStates))// || CanMoveBlock(ElementAt, Movement, StatesMask))
	{
		return true;
	}
	return false;
}

bool UGrid::CanMoveChainBlock(ABlock* Block, FVector2D const& Movement, uint8 const& IgnoreStates)
{
	FVector2D BlockPos = Block->GetPosition();
	FVector2D TargetPos = BlockPos + Movement;
	if (TargetPos.X >= 0 && TargetPos.X < NumCols)
	{
		return CanMoveBlock(Block, Movement, IgnoreStates);
	}
	return false;
}

void UGrid::OnRotate()
{
	if (IsValid(CurrentChain))
	{
		bRotate = true;
	}
}

void UGrid::OnForceDown()
{
	if (IsValid(CurrentChain))
	{
		bForceDown = true;
	}
}

void UGrid::OnMove(FVector2D const& Movement)
{
	if (IsValid(CurrentChain))
	{
		bMove = true;
		MovementInput = Movement;
	}
}

void UGrid::RotateChain()
{
	if (IsValid(CurrentChain))
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
			
			ABlock* ElementAt = GetElementAt(NewOffsetPos.X, NewOffsetPos.Y);

			if (ElementAt && ElementAt->State != EBlockState::IN_CHAIN)
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
			ABlock* ElementFrom = GetElementAt(CurrBlockPos.X, CurrBlockPos.Y);
			if (ElementFrom && ElementFrom->GetName() == CurrBlock->GetName())
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
	ABlock* ElementFrom = GetElementAt(BlockPos.X, BlockPos.Y);
	if (ElementFrom && ElementFrom->GetName() == Block->GetName())
	{
		SetElementAt(BlockPos.X, BlockPos.Y, nullptr);
	}
	SetElementAt(TargetPos.X, TargetPos.Y, Block);
	Block->SetPosition(TargetPos);
}

void UGrid::DeleteBlock(ABlock* Block)
{
	if (IsValid(Block))
	{
		FVector2D const& BlockPos = Block->GetPosition();
		SetElementAt(BlockPos.X, BlockPos.Y, nullptr);
		Block->Destroy();
	}
}

bool UGrid::CanMoveChain(FVector2D const& Movement)
{
	for (ABlock* Block : CurrentChainBlocks)
	{
		if (!CanMoveChainBlock(Block, Movement, (uint8)EBlockState::IN_CHAIN))
		{
			return false;
		}
	}
	return true;
}

void UGrid::DeleteCurrentChain()
{
	if (IsValid(CurrentChain))
	{
		for (ABlock* Block : CurrentChainBlocks)
		{
			Block->State = EBlockState::FALLING;
			//TODO: Move to another function
			FVector2D const& BlockPosition = Block->GetPosition();
			if (!IsInBounds(BlockPosition) && CanMoveBlock(Block, { 0, 1 }, (uint8)EBlockState::FALLING | (uint8)EBlockState::IN_CHAIN))
			{
				//Set bLost
			}
			NonChainBlocks.Add(Block);
		}
		bRotate = false;
		bMove = false;
		MovementInput = { .0f, .0f };
		bForceDown = false;
		CurrentChain = nullptr;
		CurrentChainBlocks.Reset(MAX_CHAIN_SIZE);
	}
}

void UGrid::ResizeBlock(ABlock* Block)
{
	float NewItemExtents = Block->GetExtents();
	float MinDimension = FMath::Min(
		(GetWidth() / (NewItemExtents * GetNumCols())) * .5f,
		(GetHeight() / (NewItemExtents * GetNumRows())) * .5f
	);
	Block->SetScale(FVector(MinDimension, MinDimension, MinDimension));
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
		if (IsValid(CurrentChain))
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
	if (IsValid(CurrentChain))
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
	for (ABlock* Block : NonChainBlocks)
	{
		if (Block->State == EBlockState::RESOLVED) {
			continue;
		}
		switch (Block->State)
		{
		case EBlockState::PENDING_RESOLUTION:
		{
			AResolvableBlock* Resolvable = Cast<AResolvableBlock>(Block);
			if (IsValid(Resolvable))
			{
				Resolvable->GetResolvedBlocks();
				//TODO: Do something with this information
			}
			Block->State = EBlockState::RESOLVED;
			break;
		}
		case EBlockState::TO_DELETE:
			DeleteBlock(Block);
			break;
		default:
		{
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
		}
	}
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

void UGrid::MoveChainToGrid(UGrid* const Other, FVector2D const& Position)
{
	Other->CurrentChain = this->CurrentChain;
	Other->CurrentChainBlocks = this->CurrentChainBlocks;
	auto const BlocksPositions = CurrentChain->GetBlocksPositions(Position);
	for (int i = 0; i < CurrentChainBlocks.Num(); i++)
	{
		FVector2D const& newPos = BlocksPositions[i];
		ABlock* Block = CurrentChainBlocks[i];
		SetElementAt(Block->GetPosition().X, Block->GetPosition().Y, nullptr);
		Other->SetElementAt(newPos.X, newPos.Y, Block);
		Block->SetGrid(Other);
		Block->SetPosition(newPos, true);
		Other->ResizeBlock(Block);
	}
	CurrentChainBlocks.Empty();
	CurrentChain = nullptr;
}

bool UGrid::IsInBounds(FVector2D const& TestPosition) const
{
	return TestPosition.X >= 0 &&
		TestPosition.X < GetNumCols() &&
		TestPosition.Y >= 0 &&
		TestPosition.Y < GetNumRows();
}

void UGrid::SpawnChain(UChain const* Chain, FVector2D const& Position)
{
	CurrentChain = Chain;
	CurrentChainBlocks.Reset(MAX_CHAIN_SIZE);
	float SpawnX = FMath::FloorToFloat(NumCols * .5f);
	TArray<FVector2D> ChainBlocksPositions = CurrentChain->GetBlocksPositions(Position);
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
		NewItem->State = EBlockState::IN_CHAIN;
		CurrentChainBlocks.Add(NewItem);
		SetElementAt(BlockPos.X, BlockPos.Y, NewItem);
		NewItem->Initialize(this, BlockPos);
		ResizeBlock(NewItem);
	}
}

ABlock* UGrid::GetElementAt(uint8 X, uint8 Y) const
{
	if (X >= 0 &&
		X < GetNumCols() &&
		Y >= 0 &&
		Y < GetNumRows())
	{
		return Grid[X + Y * NumCols];
	}
	return nullptr;
}

void UGrid::SetElementAt(uint8 X, uint8 Y, ABlock* NewElement)
{
	if (X >= 0 &&
		X < GetNumCols() &&
		Y >= 0 &&
		Y < GetNumRows())
	{
		if (IsValid(NewElement))
		{
			Grid[X + Y * NumCols] = NewElement;
		}
		else
		{
			Grid[X + Y * NumCols] = nullptr;
		}
	}
}

FVector UGrid::GetOrigin()  const
{
	return Origin;
}
