#include "Player.h"
#include "../Config.h"

Player::Player( int startRow, int startCol, TileWalkableFn isTileWalkable, StepToggledFn onStepToggled )
    : isTileWalkable( std::move( isTileWalkable ) )
    , onStepToggled( std::move( onStepToggled ) )
    , rowY( startRow )
    , colX( startCol )
{
    playerX = startCol * kTileSize;
    playerY = startRow * kTileSize;
}

void Player::movePlayer( Direction direction ) {
    switch ( direction ) {
        case Direction::Up:
            --rowY;
            if ( isTileWalkable( rowY, colX ) ) {
                playerY -= kTileSize;
                if ( onStepToggled ) onStepToggled();
            } else {
                ++rowY; // deplacement illegal -- on annule
            }
            break;

        case Direction::Down:
            ++rowY;
            if ( isTileWalkable( rowY, colX ) ) {
                playerY += kTileSize;
                if ( onStepToggled ) onStepToggled();
            } else {
                --rowY;
            }
            break;

        case Direction::Left:
            --colX;
            if ( isTileWalkable( rowY, colX ) ) {
                playerX -= kTileSize;
                if ( onStepToggled ) onStepToggled();
            } else {
                ++colX;
            }
            break;

        case Direction::Right:
            ++colX;
            if ( isTileWalkable( rowY, colX ) ) {
                playerX += kTileSize;
                if ( onStepToggled ) onStepToggled();
            } else {
                --colX;
            }
            break;
    }
}
