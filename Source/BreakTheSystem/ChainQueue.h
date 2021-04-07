// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChainQueue.generated.h"

class Chain;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREAKTHESYSTEM_API UChainQueue : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChainQueue();

	/**
	* Returns the next Chain without deleting it.
	*/
	Chain const * PeekNext();
	/**
	* Returns the next Chain and removes it from the queue.
	*/
	Chain const * GetNext();

	void Initialize(uint8 const MaxChainWidth, uint8 const MaxChainHeight, int32 const MaxChainLength);

protected:
	Chain const * CurrentChain;
	uint8 MaxChainWidth;
	uint8 MaxChainHeight;
	int32 MaxChainLength;
		
};
