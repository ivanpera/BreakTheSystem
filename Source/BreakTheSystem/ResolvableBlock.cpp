// Fill out your copyright notice in the Description page of Project Settings.


#include "ResolvableBlock.h"

TArray<ABlock*> AResolvableBlock::GetResolvedBlocks()
{
    auto result = ResolvedBlocks;
    ResolvedBlocks.Empty();
    return ResolvedBlocks;
}

void AResolvableBlock::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (State == EBlockState::IDLE)
    {
        CheckResolution();
    }
}

void AResolvableBlock::CheckResolution_Implementation()
{
}
