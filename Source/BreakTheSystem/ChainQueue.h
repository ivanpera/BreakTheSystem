// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChainQueue.generated.h"

class UChain;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREAKTHESYSTEM_API UChainQueue : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChainQueue();
	virtual void BeginDestroy() override;
	/**
	* Returns the next UChain without deleting it.
	*/
	UChain const * PeekNext();
	/**
	* Returns the current UChain without deleting it.
	*/
	UChain const* PeekCurr();
	/**
	* Removes the current UChain from the queue.
	*/
	void Pop();
	/**
	* Substitutes the current UChain with the one provided
	*/
	//void SetCurrentChain(UChain const& NewChain);
	void Initialize(uint8 const MaxChainWidth, uint8 const MaxChainHeight, int32 const MaxChainLength);

protected:
	UChain const* GetOrBuildChain(UChain const*& ChainPtr);
	UChain const * CurrentChain;
	UChain const * NextChain;
	uint8 MaxChainWidth;
	uint8 MaxChainHeight;
	int32 MaxChainLength;
		
};
