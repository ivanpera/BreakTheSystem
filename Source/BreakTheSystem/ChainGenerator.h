// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChainGenerator.generated.h"

enum class EDirection : uint8;
class ABlock;

USTRUCT()
struct FWeightedBlock
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
		TSubclassOf<ABlock> BlockType;
	UPROPERTY(EditAnywhere)
		float Weight;
	bool operator==(const FWeightedBlock& Other) const
	{
		return Equals(Other);
	}

	bool Equals(const FWeightedBlock& Other) const;
private:
	friend class UChainGenerator;
	////Used to optimize the generation of random items
	//UPROPERTY()
	//float TotalWeight;
};

#if UE_BUILD_DEBUG
uint32 GetTypeHash(const FWeightedBlock& Thing);
#else // optimize by inlining in shipping and development builds
FORCEINLINE uint32 GetTypeHash(const FWeightedBlock& Thing)
{
	uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FWeightedBlock));
	return Hash;
}
#endif

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREAKTHESYSTEM_API UChainGenerator : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	UFUNCTION(BluePrintCallable)
		class UChain* GenerateChain(FVector2D const& SpawnArea, int32 MaxNumBlocks);
private:
	TSubclassOf<class ABlock> GetRandomBlockType();
	void GenerateChildren(class UChainItem* Node, FVector2D const& SpawnArea, TSet<FString>& OccupiedPositions, int32& MaxNumBlocks, float const SpawnProbabilityModifier = 1.0f);
	TMap<EDirection, FVector2D> const GetValidChildrenPositions(class UChainItem const* Node, FVector2D const& SizeLimits, TSet<FString>& OccupiedPositions) const;
	UPROPERTY(EditAnywhere)
		TSet<FWeightedBlock> WeightedBlocks;
	float TotalWeight;
};
