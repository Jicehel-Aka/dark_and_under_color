#include "Minimap.h"
#include "../levels/Level.h"
#include "../entities/Player.h"
#include "../entities/Object.h"
#include "../entities/Enemy.h"
#include "../Config.h"
#include <cmath>

namespace {
constexpr int16_t kCellSize = kTileSize * 2; // zoom AKA x2
constexpr int16_t kMapOriginX = 47 * 2;
constexpr int16_t kMapOriginY = 35 * 2;
}

void Minimap::render( IRenderer& renderer, const Level& level, const Player& player,
                       FacingDirection facing,
                       const std::vector<GameObject>& objects, const std::vector<Enemy>& enemies ) {
    renderer.drawImageScaled( 0, 0, 150 * 2, 81 * 2, (ImageId)ImageAsset::MinimapParchemin );

    const int playerRow = player.getRow();
    const int playerCol = player.getCol();

    for ( int row = playerRow - 4; row <= playerRow + 4; ++row ) {
        for ( int col = playerCol - 4; col <= playerCol + 4; ++col ) {
            TileType tile = level.getData().tileAt( row, col );
            if ( tile == TileType::Floor ) continue; // rien a dessiner, comme l'original (noFill())

            int16_t x = (int16_t)( kMapOriginX + ( col - playerCol ) * kCellSize );
            int16_t y = (int16_t)( kMapOriginY + ( row - playerRow ) * kCellSize );
            renderer.drawImageScaled( x, y, kCellSize, kCellSize, (ImageId)ImageAsset::WallMapMarker );
        }
    }

    ImageAsset playerMarker = ImageAsset::PlayerMapMarker1; // valeur par defaut -- evite un avertissement -Werror=maybe-uninitialized que GCC ne peut pas lever ici (switch exhaustif sur les 4 seules valeurs de l'enum, mais indetectable pour lui)
    switch ( facing ) {
        case FacingDirection::North: playerMarker = ImageAsset::PlayerMapMarker1; break;
        case FacingDirection::East:  playerMarker = ImageAsset::PlayerMapMarker2; break;
        case FacingDirection::South: playerMarker = ImageAsset::PlayerMapMarker3; break;
        case FacingDirection::West:  playerMarker = ImageAsset::PlayerMapMarker4; break;
    }
    renderer.drawImageScaled( kMapOriginX, kMapOriginY, kCellSize, kCellSize, (ImageId)playerMarker );

    for ( const GameObject& o : objects ) {
        if ( !o.isActive() ) continue;
        if ( std::abs( o.getCol() - playerCol ) < 5 && std::abs( o.getRow() - playerRow ) < 5 ) {
            int16_t x = (int16_t)( kMapOriginX + ( o.getCol() - playerCol ) * kCellSize );
            int16_t y = (int16_t)( kMapOriginY + ( o.getRow() - playerRow ) * kCellSize );
            renderer.drawImageScaled( x, y, kCellSize, kCellSize, (ImageId)ImageAsset::ObjectMapMarker );
        }
    }

    for ( const Enemy& e : enemies ) {
        if ( std::abs( e.getCol() - playerCol ) < 5 && std::abs( e.getRow() - playerRow ) < 5 ) {
            int16_t x = (int16_t)( kMapOriginX + ( e.getCol() - playerCol ) * kCellSize );
            int16_t y = (int16_t)( kMapOriginY + ( e.getRow() - playerRow ) * kCellSize );
            renderer.drawImageScaled( x, y, kCellSize, kCellSize, (ImageId)ImageAsset::EnemyMapMarker );
        }
    }
}
