#pragma once
#include "CoreMinimal.h"
#include "ChainItem.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	NO_DIRECTION,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

UCLASS()
class UChainItem : public UObject
{
	GENERATED_BODY()

public:
	UChainItem() = default;
	void Initialize(EDirection const& Direction, UChainItem* Parent, FVector2D const& BlockPosition, TSubclassOf<class ABlock> BlockType);
	UPROPERTY()
	EDirection Direction;
	UPROPERTY()
	TArray<UChainItem*> Children;
	UPROPERTY()
	UChainItem* Parent;
	UPROPERTY()
	TSubclassOf<class ABlock> BlockType;
private:
	//To be changed to ChainGeneratorComponent
	friend class UChain;
	friend class UChainGenerator;
private:
	UPROPERTY()
	FVector2D BlockPosition; //Only used in chain generation
};