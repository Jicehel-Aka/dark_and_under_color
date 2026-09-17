#include "Object.h"
#include "../Config.h"

GameObject::GameObject( int colX, int rowY, ObjectType type, const char* id )
    : objectRow( rowY )
    , objectCol( colX )
    , objectType( type )
    , id( id )
{
    objectX = colX * kTileSize;
    objectY = rowY * kTileSize;
}

bool GameObject::checkCollision( int checkCol, int checkRow, int playerCol, int playerRow ) const {
    return ( checkCol == playerCol ) && ( checkRow == playerRow );
}
