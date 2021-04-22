// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

class ABlock;

enum class EBlockState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGridChainReached);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BREAKTHESYSTEM_API UGrid : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	UGrid();
	uint8 GetNumCols() const;
	uint8 GetNumRows() const;
	float GetWidth() const;
	float GetHeight() const;
	float GetStepX() const;
	float GetStepY() const;
	FVector GetOrigin() const;
	bool IsInBounds(FVector2D const& Position) const;

protected:
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FGridChainReached OnChainStop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float UpdateTimeSeconds;
	UPROPERTY(BlueprintReadOnly)
		uint8 NumCols;
	UPROPERTY(BlueprintReadOnly)
		uint8 NumRows;
	UPROPERTY(BlueprintReadOnly)
		float Width;
	UPROPERTY(BlueprintReadOnly)
		float Height;
	UPROPERTY(BlueprintReadOnly)
		float StepX;
	UPROPERTY(BlueprintReadOnly)
		float StepY;
	UPROPERTY(BlueprintReadOnly)
		float RemainingTime;
	UPROPERTY(BlueprintReadOnly)
		TArray<ABlock*> Grid;
	UPROPERTY(BlueprintReadOnly)
		class UChain const* CurrentChain;

	void ResizeBlock(ABlock* Block);
	void MoveChain(FVector2D const& Movement);
	void MoveBlock(class ABlock* Block, FVector2D const& Movement);
	void DeleteBlock(class ABlock* Block);
	bool CanMoveChain(FVector2D const& Movement);
	bool CanMoveBlock(class ABlock* Block, FVector2D const& Movement, uint8 const& StatesMask);
	bool CanMoveChainBlock(class ABlock* Block, FVector2D const& Movement, uint8 const& StatesMask);
	void DeleteCurrentChain();
	void SetElementAt(uint8 X, uint8 Y, ABlock* NewElement);
	void RotateChain();
	void FallChainBlocks(float DeltaTime);
	void HandlePlayerMovement();
	FVector Origin;
	TArray<ABlock*> NonChainBlocks;
	TArray<ABlock*> CurrentChainBlocks;
	FVector2D MovementInput;
	float UpdateTimeLeft;
	bool bRotate;
	bool bMove;
	bool bForceDown;

public:
	UFUNCTION(BlueprintCallable)
		void UpdateGrid(float DeltaTime);
	UFUNCTION(BlueprintCallable)
		void SpawnChain(class UChain const* UChain, FVector2D const& Position);
	UFUNCTION(BlueprintCallable)
		void Initialize(uint8 Rows, uint8 Cols, FVector const& Centre, FVector const& Extents);
	UFUNCTION(BlueprintCallable)
		ABlock* GetElementAt(uint8 X, uint8 Y) const;
	UFUNCTION(BlueprintCallable)
		void OnRotate();
	UFUNCTION(BlueprintCallable)
		void OnMove(FVector2D const& Movement);
	UFUNCTION(BlueprintCallable)
		void OnForceDown();
	UFUNCTION(BlueprintCallable)
		void MoveChainToGrid(UGrid* const Other, FVector2D const& Position);
};
