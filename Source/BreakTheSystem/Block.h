// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.generated.h"

class UGrid;

UENUM(BlueprintType)
enum class EBlockState : uint8
{
	NO_STATE = 0 UMETA(Hidden), //0 state, to make masking with other states easier
	FALLING = 1 << 0,
	IN_CHAIN = 1 << 1,
	IDLE = 1 << 2,
	PENDING_RESOLUTION = 1 << 3,
	RESOLVED = 1 << 4,
	TO_DELETE = 1 << 5
};

UCLASS()
class BREAKTHESYSTEM_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		FVector2D GetPosition() const;
public:
	virtual void Tick(float DeltaSeconds) override;
	// Sets default values for this actor's properties
	ABlock();
	float GetExtents() const;
	void Initialize(UGrid* Grid, FVector2D NewPosition);
	void SetPosition(FVector2D const& NewPos, bool bSnap = false);
	void SetScale(FVector const& Scale);
	void SetGrid(UGrid* const Grid);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TransitionSpeedSeconds;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		EBlockState State;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector2D Position;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		class USceneComponent* Root;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		class UStaticMeshComponent* Mesh;
	UPROPERTY(BlueprintReadOnly)
		UGrid* Grid;
protected:
	void Translate(float DeltaSeconds);
protected:
	float Extents;
};
