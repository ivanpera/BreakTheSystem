// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

UCLASS()
class BREAKTHESYSTEM_API AGrid : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AGrid();
	uint8 GetNumCols() const;
	uint8 GetNumRows() const;
	float GetWidth() const;
	float GetHeight() const;
	float GetStepX() const;
	float GetStepY() const;
	class UChainQueue const* GetChainQueue() const;
	FVector GetOrigin() const;
	UFUNCTION(BlueprintCallable)
		void OnRotate();
	UFUNCTION(BlueprintCallable)
		void OnMove(FVector2D const& Movement);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float UpdateTimeSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 NumCols;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
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
	//Components
	class USceneComponent* RootComponent;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		class UBoxComponent* Box;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		class UBoxComponent* PreviewBox;
	//Grids
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<class ABlock*> Grid;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<class ABlock*> PreviewGrid;

	//To delete
	UPROPERTY(EditAnywhere)
		TSubclassOf<ABlock> BlockType;

	void MoveChain(FVector2D const& Movement);
	void MoveBlock(class ABlock* Block, FVector2D const& Movement);
	bool CanMoveChain(FVector2D const& Movement);
	bool CanMoveBlock(class ABlock* Block, FVector2D const& Movement, uint8 const& StatesMask);
	void DeleteCurrentChain();
	void SetElementAt(uint8 X, uint8 Y, class ABlock* NewElement);
	void UpdateGrid(float DeltaTime);
	bool IsInBounds(FVector2D const& Position) const;
	void RotateChain();
	void FallChainBlocks(float DeltaTime);
	void HandlePlayerMovement();
	class UChainQueue* ChainQueue;
	class Chain const* CurrentChain;
	FVector Origin;
	TArray<ABlock*> CurrentChainBlocks;
	FVector2D MovementInput;
	float UpdateTimeLeft;
	bool bUpdateGravity;
	bool bRotate;
	bool bMove;
	bool bForceDown;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable)
		void SpawnChain();

	UFUNCTION(BlueprintCallable)
		class ABlock* GetElementAt(uint8 X, uint8 Y) const;

};
