#include "Vision.h"
#include "../levels/Level.h"
#include "../entities/Player.h"
#include "../entities/Object.h"
#include "../entities/Enemy.h"
#include "../Config.h"

FacingDirection rotateRight( FacingDirection d ) {
    int v = (int)d + 1;
    if ( v > 4 ) v = 1;
    return (FacingDirection)v;
}
FacingDirection rotateLeft( FacingDirection d ) {
    int v = (int)d - 1;
    if ( v < 1 ) v = 4;
    return (FacingDirection)v;
}

namespace {

struct Vec { int drow, dcol; };

// vecteur_avant pour chaque direction -- vecteur_droite = rotation 90°
// de vecteur_avant, voir Vision.h pour la demonstration.
Vec forwardVector( FacingDirection d ) {
    switch ( d ) {
        case FacingDirection::North: return { -1, 0 };
        case FacingDirection::East:  return { 0, 1 };
        case FacingDirection::South: return { 1, 0 };
        case FacingDirection::West:  return { 0, -1 };
    }
    return { -1, 0 };
}
Vec rightVector( Vec forward ) {
    return { forward.dcol, -forward.drow };
}

// Les 11 cases verifiees par tour, en coordonnees (forward, right)
// relatives au joueur -- table verifiee case par case contre les 4
// fonctions lookForward/lookRight/lookBack/lookLeft de l'original (voir
// Vision.h). variant2 == variant1 pour les 3 cases qui n'ont jamais de
// texture alternative (far front à 3 cases, et les 2 coins "farest").
struct WallCheck {
    int forward, right;
    ImageAsset variant1;
    ImageAsset variant2;
};

const WallCheck kWallChecks[11] = {
    { 3,  0, ImageAsset::FarWallFront,    ImageAsset::FarWallFront },
    { 3, -1, ImageAsset::FarestWallLeft,  ImageAsset::FarestWallLeft },
    { 3,  1, ImageAsset::FarestWallRight, ImageAsset::FarestWallRight },
    { 2, -1, ImageAsset::FarWallLeft,     ImageAsset::FarWallLeft2 },
    { 2,  1, ImageAsset::FarWallRight,    ImageAsset::FarWallRight2 },
    { 2,  0, ImageAsset::MidWallFront,    ImageAsset::MidWallFront2 },
    { 1, -1, ImageAsset::MidWallLeft,     ImageAsset::MidWallLeft2 },
    { 1,  1, ImageAsset::MidWallRight,    ImageAsset::MidWallRight2 },
    { 1,  0, ImageAsset::CloseWallFront,  ImageAsset::CloseWallFront2 },
    { 0, -1, ImageAsset::CloseWallLeft,   ImageAsset::CloseWallLeft2 },
    { 0,  1, ImageAsset::CloseWallRight,  ImageAsset::CloseWallRight2 },
};

constexpr int16_t kWallW = 98 * 2; // 98x70 logique, zoom AKA x2 -- voir Config.h/main
constexpr int16_t kWallH = 70 * 2;
constexpr int16_t kOriginX = 3 * 2;
constexpr int16_t kOriginY = 4 * 2;

} // namespace

void Vision::renderWalls( IRenderer& renderer, const Level& level, int playerRow, int playerCol ) {
    Vec fwd = forwardVector( direction );
    Vec right = rightVector( fwd );

    for ( const WallCheck& check : kWallChecks ) {
        int row = playerRow + check.forward * fwd.drow + check.right * right.drow;
        int col = playerCol + check.forward * fwd.dcol + check.right * right.dcol;

        TileType tile = level.getData().tileAt( row, col );
        if ( tile == TileType::Floor ) continue; // rien a dessiner, case franchissable

        ImageAsset asset = ( tile == TileType::Special ) ? check.variant2 : check.variant1;
        renderer.drawImageScaled( kOriginX, kOriginY, kWallW, kWallH, (ImageId)asset );
    }
}

void Vision::render( IRenderer& renderer, const Level& level, const Player& player,
                      const std::vector<GameObject>& objects, const std::vector<Enemy>& enemies ) {
    ImageAsset back = oddStep ? ImageAsset::VisionBack : ImageAsset::VisionBack2;
    renderer.drawImageScaled( kOriginX, kOriginY, kWallW, kWallH, (ImageId)back );

    renderWalls( renderer, level, player.getRow(), player.getCol() );
    renderObjects( renderer, level, player.getRow(), player.getCol(), objects );
    renderEnemies( renderer, level, player.getRow(), player.getCol(), enemies );
}

// ---------------------------------------------------------------------
// Objets et ennemis visibles dans le couloir.
//
// Règle de visibilité de l'original (identique pour les deux, un objet
// n'est visible que s'il est EXACTEMENT dans l'axe où regarde le joueur
// -- même ligne ou même colonne selon la direction -- à 2 cases ou
// moins, et si la case immédiatement devant le joueur n'est pas un mur
// (sinon la vue est bouchée). Pas de vraie occlusion au-delà de cette
// première case -- fidèle à l'original, qui a la même limite.
// ---------------------------------------------------------------------

namespace {

constexpr int16_t kMedIconSize = 16 * 2;
constexpr int16_t kSmallIconSize = 8 * 2;
constexpr int16_t kMedIconX = 50 * 2;
constexpr int16_t kSmallIconX = 50 * 2;

// true si (targetRow,targetCol) est visible depuis (playerRow,playerCol)
// dans la direction "direction" -- reproduit les 4 blocs if/else de
// displayObjects()/displayEnemies() de l'original (identiques pour
// objets et ennemis, factorisés ici en une seule fonction).
bool isVisibleAhead( FacingDirection direction, const Level& level,
                      int playerRow, int playerCol, int targetRow, int targetCol,
                      int& outDistance ) {
    switch ( direction ) {
        case FacingDirection::North:
            if ( targetCol != playerCol || targetRow >= playerRow ) return false;
            if ( level.getData().tileAt( playerRow - 1, playerCol ) != TileType::Floor ) return false;
            outDistance = playerRow - targetRow;
            break;
        case FacingDirection::South:
            if ( targetCol != playerCol || targetRow <= playerRow ) return false;
            if ( level.getData().tileAt( playerRow + 1, playerCol ) != TileType::Floor ) return false;
            outDistance = targetRow - playerRow;
            break;
        case FacingDirection::East:
            if ( targetRow != playerRow || targetCol <= playerCol ) return false;
            if ( level.getData().tileAt( playerRow, playerCol + 1 ) != TileType::Floor ) return false;
            outDistance = targetCol - playerCol;
            break;
        case FacingDirection::West:
            if ( targetRow != playerRow || targetCol >= playerCol ) return false;
            if ( level.getData().tileAt( playerRow, playerCol - 1 ) != TileType::Floor ) return false;
            outDistance = playerCol - targetCol;
            break;
    }
    return outDistance <= 2; // au-dela de 2 cases, l'original ne dessine plus rien
}

} // namespace

void Vision::renderObjects( IRenderer& renderer, const Level& level, int playerRow, int playerCol,
                             const std::vector<GameObject>& objects ) {
    for ( const GameObject& o : objects ) {
        if ( !o.isActive() ) continue; // suppose une convention "actif tant que non ramasse"

        int distance;
        if ( !isVisibleAhead( direction, level, playerRow, playerCol, o.getRow(), o.getCol(), distance ) )
            continue;

        // distance 0 ou 1 -> icone moyenne, distance 2 -> icone petite
        // (meme regle que showMedObject/showSmallObject de l'original,
        // le type exact d'objet determine l'ImageAsset -- table a
        // completer une fois les ObjectType->ImageAsset(Med/Small)
        // toutes couvertes par AssetIds.h).
        ImageAsset asset = ImageAsset::LootPotionMed; // valeur par defaut -- voir Minimap.cpp pour l'explication (-Werror=maybe-uninitialized)
        bool useSmall = ( distance == 2 );
        switch ( o.getType() ) {
            case ObjectType::Potion: asset = useSmall ? ImageAsset::LootPotionSmall : ImageAsset::LootPotionMed; break;
            case ObjectType::Key:    asset = useSmall ? ImageAsset::LootKeySmall    : ImageAsset::LootKeyMed;    break;
            case ObjectType::Magic:  asset = useSmall ? ImageAsset::LootMagicSmall  : ImageAsset::LootMagicMed;  break;
            default: continue;
        }
        int16_t size = useSmall ? kSmallIconSize : kMedIconSize;
        int16_t x = useSmall ? kSmallIconX : kMedIconX;
        renderer.drawImageScaled( x, kOriginY + 41 * 2, size, size, (ImageId)asset );

        // NOTE : le déclenchement du dialogue "objet ramassé" quand le
        // joueur est EXACTEMENT sur la case de l'objet (checkCollision)
        // n'est pas fait ici -- Vision ne connaît pas Dialogue (même
        // découplage que partout ailleurs dans ce portage). C'est à
        // l'appelant (boucle principale) de tester
        // o.checkCollision(...) après ce rendu et de déclencher le
        // dialogue lui-même.
    }
}

void Vision::renderEnemies( IRenderer& renderer, const Level& level, int playerRow, int playerCol,
                             const std::vector<Enemy>& enemies ) {
    for ( const Enemy& e : enemies ) {
        int distance;
        if ( !isVisibleAhead( direction, level, playerRow, playerCol, e.getRow(), e.getCol(), distance ) )
            continue;

        bool useSmall = ( distance == 2 );
        ImageAsset asset = ImageAsset::EnemyRatIconMed; // valeur par defaut -- voir Minimap.cpp
        switch ( e.getType() ) {
            case EnemyType::Rat:      asset = useSmall ? ImageAsset::EnemyRatIconSmall      : ImageAsset::EnemyRatIconMed;      break;
            case EnemyType::Skeleton: asset = useSmall ? ImageAsset::EnemySkeletonIconSmall  : ImageAsset::EnemySkeletonIconMed; break;
            default: continue;
        }
        int16_t size = useSmall ? kSmallIconSize : kMedIconSize;
        int16_t x = useSmall ? kSmallIconX : kMedIconX;
        renderer.drawImageScaled( x, kOriginY + 36 * 2, size, size, (ImageId)asset );

        // NOTE : même remarque que pour les objets -- le déclenchement
        // du combat (e.checkCollision) est à tester par l'appelant.
    }
}
