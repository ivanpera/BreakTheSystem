// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.h"
#include "ResolvableBlock.generated.h"

/**
 * 
 */
UCLASS()
class BREAKTHESYSTEM_API AResolvableBlock : public ABlock
{
	GENERATED_BODY()
public:
	TArray<ABlock*> GetResolvedBlocks();
	virtual void Tick(float DeltaSeconds) override;
protected:
	UFUNCTION(BlueprintNativeEvent)
		void CheckResolution();
protected:
	virtual void CheckResolution_Implementation();
private:
	TArray<ABlock*> ResolvedBlocks;
};
