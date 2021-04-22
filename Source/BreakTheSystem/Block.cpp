// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include "Grid.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
ABlock::ABlock()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    Root = CreateDefaultSubobject<USceneComponent>("Root");
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
    SetRootComponent(Root);
    FAttachmentTransformRules AttachRules(EAttachmentRule::KeepRelative, false);
    Mesh->AttachToComponent(Root, AttachRules);
    Grid = nullptr;
}

void ABlock::SetScale(FVector const& Scale)
{
    SetActorScale3D(Scale);
}

FVector2D ABlock::GetPosition() const
{
    return Position;
}

void ABlock::SetGrid(UGrid* const NewGrid)
{
    Grid = NewGrid;
}

void ABlock::Initialize(UGrid* NewGrid, FVector2D NewPosition)
{
    this->Grid = NewGrid;
    this->Position = NewPosition;
    FVector BoxExtents = Mesh->Bounds.BoxExtent;
    Extents = FMath::Min(BoxExtents.Y, BoxExtents.Z);
}

void ABlock::Tick(float DeltaSeconds)
{
    if (Grid)
    {
        SetActorHiddenInGame(!Grid->IsInBounds(Position));
        Translate(DeltaSeconds);
        if (State == EBlockState::RESOLVED)
        {
            //TODO: Play animation (or whatever)
            State = EBlockState::TO_DELETE;
        }
    }
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

void ABlock::SetPosition(FVector2D const& NewPos, bool bSnap)
{
    Position = { FMath::FloorToFloat(NewPos.X), FMath::FloorToFloat(NewPos.Y) };
    if (bSnap)
    {
        FVector TargetLocation = Grid->GetOrigin() + FVector(.0f, Position.X * Grid->GetStepX(), -Position.Y * Grid->GetStepY());
        FVector NewLocation = GetActorLocation() + (TargetLocation - GetActorLocation());
        SetActorLocation(NewLocation);
    }
}

float ABlock::GetExtents() const
{
    return Extents;
}


