// Fill out your copyright notice in the Description page of Project Settings.


#include "ChainQueue.h"
#include "Chain.h"
#include "Grid.h"

// Sets default values for this component's properties
UChainQueue::UChainQueue()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UChainQueue::Initialize(uint8 const _MaxChainWidth, uint8 const _MaxChainHeight, int32 const _MaxChainLength)
{
	MaxChainLength = _MaxChainLength;
	MaxChainWidth = _MaxChainWidth;
	MaxChainHeight = _MaxChainHeight;
}

Chain const* UChainQueue::PeekNext()
{
	if (!CurrentChain)
	{
		CurrentChain = Chain::GenerateChain({ MaxChainWidth * 1.0f, MaxChainHeight * 1.0f }, MaxChainLength);
	}
	return CurrentChain;
}

Chain const* UChainQueue::GetNext()
{
	Chain const* Result = PeekNext();
	CurrentChain = nullptr;
	return Result;
}


