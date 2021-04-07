// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "ChainQueue.h"
#include "Block.h"
#include "Chain.h"

static float constexpr MAX_CHAIN_SIZE = 4;
static float constexpr DEFAULT_NUM_ROWS = 11;
static float constexpr DEFAULT_NUM_COLS = 11;

// Sets default values
AGrid::AGrid() : CurrentChain(nullptr)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewBox = CreateDefaultSubobject<UBoxComponent>("PreviewBox");
	PreviewBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChainQueue = CreateDefaultSubobject<UChainQueue>("ChainQueue");
	SetRootComponent(RootComponent);
}

bool AGrid::CanMoveBlock(class ABlock* Block, FVector2D const& Movement, uint8 const& IgnoreStates)
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

void AGrid::OnRotate()
{
	if (CurrentChain)
	{
		bRotate = true;
	}
}

void AGrid::OnMove(FVector2D const& Movement)
{
	if (CurrentChain)
	{
		if (Movement.Y > 0)
		{
			bForceDown = true;
		}
		else
		{
			bMove = true;
			MovementInput = Movement;
		}
	}
}

void AGrid::RotateChain()
{
	if (CurrentChain)
	{
		Chain* Rotated = new Chain(CurrentChain->Rotate());
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
			if (ElementFrom == CurrBlock)
			{
				SetElementAt(CurrBlockPos.X, CurrBlockPos.Y, nullptr);
			}
			SetElementAt(TargetPos.X, TargetPos.Y, CurrBlock);
			CurrBlock->SetPosition(TargetPos);
		}
		delete CurrentChain;
		CurrentChain = Rotated;
	}
}

void AGrid::MoveBlock(ABlock* Block, FVector2D const& Movement)
{
	FVector2D BlockPos = Block->GetPosition();
	FVector2D TargetPos = BlockPos + Movement;
	ABlock* ElementFrom = GetElementAt(BlockPos.X, BlockPos.Y);
	if (ElementFrom == Block)
	{
		SetElementAt(BlockPos.X, BlockPos.Y, nullptr);
	}
	SetElementAt(TargetPos.X, TargetPos.Y, Block);
	Block->SetPosition(TargetPos);
}

bool AGrid::CanMoveChain(FVector2D const& Movement)
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

void AGrid::DeleteCurrentChain()
{
	for (ABlock* Block : CurrentChainBlocks)
	{
		Block->State = EBlockState::FALLING;
		//TODO: Move to another function
		FVector2D const& BlockLocation = Block->GetPosition();
		if (BlockLocation.Y < 0 && CanMoveBlock(Block, { BlockLocation.X, 0 }, (uint8)EBlockState::FALLING | (uint8)EBlockState::IN_CHAIN))
		{
			//Set bLost
			//Block->SetPosition({ BlockLocation.X, 0.0f });
		}
	}
	delete CurrentChain;
	CurrentChain = nullptr;
	bRotate = false;
	bUpdateGravity = true;
	bMove = false;
	MovementInput = { .0f, .0f };
	bForceDown = false;
}

void AGrid::MoveChain(FVector2D const& Movement)
{
	for (ABlock* Block : CurrentChainBlocks)
	{
		MoveBlock(Block, Movement);
	}
}

void AGrid::FallChainBlocks(float DeltaTime)
{
	if (UpdateTimeLeft <= 0 || bForceDown)
	{
		//UE_LOG()
		if (CurrentChain)
		{
			//TODO: If last Forced input < threshold, move down each chain block
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
		else
		{
			SpawnChain();
		}
	}
	else
	{
		UpdateTimeLeft -= DeltaTime;
	}
}

void AGrid::HandlePlayerMovement()
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

void AGrid::UpdateGrid(float DeltaTime)
{
	FallChainBlocks(DeltaTime);
	HandlePlayerMovement();
	/*if (bUpdateGravity)
	{*/
		for (ABlock* Block : Grid)
		{
			if (!Block || Block->State == EBlockState::IN_CHAIN || Block->State == EBlockState::PENDING_RESOLUTION) {
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
		bUpdateGravity = false;
	//if (!CanMoveChain({ 0.0f, 1.0f }))
	//{
	//	DeleteCurrentChain();
	//}
	//}

	//Resolve blocks
}

UChainQueue const* AGrid::GetChainQueue() const
{
	return ChainQueue;
}

uint8 AGrid::GetNumCols() const
{
	return NumCols;
}

uint8 AGrid::GetNumRows() const
{
	return NumRows;
}

float AGrid::GetWidth() const
{
	return Width;
}

float AGrid::GetHeight() const
{
	return Height;
}

float AGrid::GetStepX() const
{
	return StepX;
}

float AGrid::GetStepY() const
{
	return StepY;
}

void AGrid::BeginDestroy()
{
	Super::BeginDestroy();
	delete CurrentChain;
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	FVector Extents = Box->Bounds.GetBox().GetExtent();
	Width = Extents.Y * 2.0f;
	Height = Extents.Z * 2.0f;
	float const DividerX = NumCols <= 0 ? 1.0f : NumCols * 1.0f;
	float const DividerY = NumRows <= 0 ? 1.0f : NumRows * 1.0f;
	Grid.Init(nullptr, NumCols * (NumRows + MAX_CHAIN_SIZE));
	PreviewGrid.Init(nullptr, MAX_CHAIN_SIZE * MAX_CHAIN_SIZE);
	CurrentChainBlocks.Init(nullptr, MAX_CHAIN_SIZE);
	StepX = Width / DividerX;
	StepY = Height / DividerY;
	Origin = Box->Bounds.GetBox().GetCenter()
		+ (FVector(0.0f, -Extents.Y, Extents.Z)
			+ FVector(0.0f, StepX * 0.5f, -StepY * 0.5f));
	ChainQueue->Initialize(MAX_CHAIN_SIZE, MAX_CHAIN_SIZE, MAX_CHAIN_SIZE);
}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateGrid(DeltaTime);
}

bool AGrid::IsInBounds(FVector2D const& TestPosition) const
{
	return TestPosition.X >= 0 &&
		TestPosition.X < GetNumCols() &&
		TestPosition.Y >= 0 &&
		TestPosition.Y < GetNumRows();
}

void AGrid::SpawnChain()
{
	delete CurrentChain;
	CurrentChain = ChainQueue->GetNext();
	CurrentChainBlocks.Reset(MAX_CHAIN_SIZE);
	float SpawnX = FMath::FloorToFloat(NumCols * .5f);
	TArray<FVector2D> ChainBlocksPositions = CurrentChain->GetBlocksPositions({SpawnX, -1.f});
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform Transform;
	if (BlockType)
	{
		for (auto const& BlockPos : ChainBlocksPositions)
		{
			//If the position is blocked, it's game over
			Transform.SetLocation(Origin + FVector(0.0f, BlockPos.X * StepX, -BlockPos.Y * StepY));
			auto NewItem = GetWorld()->SpawnActor<ABlock>(BlockType, Transform, SpawnParams);
			float NewItemExtents = NewItem->GetExtents();
			NewItem->State = EBlockState::IN_CHAIN;
			float MinDimension = FMath::Min((Width / (NewItemExtents * NumCols)) * .5f, (Height / (NewItemExtents * NumRows)) * .5f);
			NewItem->SetActorScale3D(FVector(1.0f, MinDimension, MinDimension));
			NewItem->Initialize(this, BlockPos);
			CurrentChainBlocks.Add(NewItem);
			SetElementAt(BlockPos.X, BlockPos.Y, NewItem);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing BlockType"));
	}
}

ABlock* AGrid::GetElementAt(uint8 X, uint8 Y) const
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

void AGrid::SetElementAt(uint8 X, uint8 Y, ABlock* NewElement)
{
	if (X >= 0 &&
		X < GetNumCols() &&
		Y >= 0 &&
		Y < GetNumRows())
	{
		Grid[X + Y * NumCols] = NewElement;
	}
}

FVector AGrid::GetOrigin()  const
{
	return Origin;
}
