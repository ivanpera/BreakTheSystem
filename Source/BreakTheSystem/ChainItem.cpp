#include "ChainItem.h"
#include "Block.h"

void UChainItem::Initialize(EDirection const& _Direction, UChainItem* _Parent, FVector2D const& _BlockPosition, TSubclassOf<ABlock> _BlockType)
{
    Direction = _Direction;
    Parent = _Parent;
    BlockPosition = _BlockPosition;
    BlockType = _BlockType;
}