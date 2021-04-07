// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include "Grid.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABlock::ABlock()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
    this->SetRootComponent(Mesh);
    Grid = nullptr;
}

FVector2D ABlock::GetPosition() const
{
    return Position;
}

void ABlock::Initialize(AGrid* NewGrid, FVector2D NewPosition)
{
    this->Grid = NewGrid;
    this->Position = NewPosition;
}

void ABlock::Tick(float DeltaSeconds)
{
    Translate(DeltaSeconds);
}

void ABlock::Translate(float DeltaSeconds)
{
    if (Grid)
    {
        FVector TargetLocation = Grid->GetOrigin() + FVector(.0f, Position.X * Grid->GetStepX(), -Position.Y * Grid->GetStepY());
        if (!FVector::PointsAreNear(GetActorLocation(), TargetLocation, 1.0f))
        {
            float Speed = TransitionSpeedSeconds == 0.0f ? .1f : TransitionSpeedSeconds;
            FVector NewLocation;
            NewLocation = GetActorLocation() + ((TargetLocation - GetActorLocation()) / Speed) * DeltaSeconds;
            SetActorLocation(NewLocation);
        }
        else
        {
            SetActorLocation(TargetLocation);
        }
    }
}

void ABlock::SetPosition(FVector2D const& NewPos)
{
    Position = NewPos;
}

float ABlock::GetExtents() const
{
    return Mesh->Bounds.SphereRadius;
}


