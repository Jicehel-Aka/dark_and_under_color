#include "UI.h"
#include "Player.h"
#include "Enemy.h"
#include "rendering/Vision.h" // FacingDirection
#include <cstdio>

void UI::renderFrame( IRenderer& renderer ) {
    // Cadre/fond -- inclut le panneau de droite (dessine dans l'image
    // elle-meme), doit passer AVANT Vision::render() pour que le
    // couloir se dessine PAR-DESSUS, pas l'inverse.
    renderer.drawImageScaled( 0, 0, 150 * 2, 81 * 2, (ImageId)ImageAsset::UIMain );
}

void UI::renderStatus( IRenderer& renderer, const Player& player, FacingDirection facing ) {
    // Icones de direction (D-pad) -- gardees comme repere visuel statique
    // (retour de Jicehel apres test reel : visibles et blanches dans
    // l'original, plus lisibles ainsi) meme si non cliquables ici (de
    // vrais boutons physiques remplacent le clic). Memes positions que
    // l'original (UI.pde : Buttons(...) constructeurs).
    renderer.drawImageScaled( 118 * 2, 51 * 2, 12 * 2, 12 * 2, (ImageId)ImageAsset::ArrowForward );
    renderer.drawImageScaled( 118 * 2, 66 * 2, 12 * 2, 12 * 2, (ImageId)ImageAsset::ArrowBackward );
    renderer.drawImageScaled( 103 * 2, 66 * 2, 12 * 2, 12 * 2, (ImageId)ImageAsset::ArrowLeft );
    renderer.drawImageScaled( 133 * 2, 66 * 2, 12 * 2, 12 * 2, (ImageId)ImageAsset::ArrowRight );
    renderer.drawImageScaled( 133 * 2, 51 * 2, 12 * 2, 12 * 2, (ImageId)ImageAsset::ArrowRotateRight );
    renderer.drawImageScaled( 103 * 2, 51 * 2, 12 * 2, 12 * 2, (ImageId)ImageAsset::ArrowRotateLeft );

    // Icones d'action rapide (grille 2x2 sous PV/XP) -- memes positions
    // que l'original (UI.pde : swordAttackButton/magicAttackButton/
    // potionButton/shieldButton), reperes visuels statiques.
    renderer.drawImageScaled( 110 * 2, 18 * 2, 12 * 2, 12 * 2, (ImageId)ImageAsset::IconSword );
    renderer.drawImageScaled( 125 * 2, 18 * 2, 12 * 2, 12 * 2, (ImageId)ImageAsset::IconMagic );
    renderer.drawImageScaled( 110 * 2, 34 * 2, 12 * 2, 12 * 2, (ImageId)ImageAsset::IconPotion );
    renderer.drawImageScaled( 125 * 2, 34 * 2, 12 * 2, 12 * 2, (ImageId)ImageAsset::IconShield );

    // Barres PV/XP -- meme echelle que l'original (17 unites de large
    // max), meme position relative.
    float hpRatio = player.getCurrentHP() / player.getMaxHP();
    float xpRatio = player.getCurrentXP() / player.getMaxXP();
    renderer.fillRect( 116 * 2, 8 * 2, (int16_t)( hpRatio * 17 * 2 ), 3 * 2, RGBColor{ 0xff, 0x48, 0x00 } );
    renderer.fillRect( 116 * 2, 12 * 2, (int16_t)( xpRatio * 17 * 2 ), 3 * 2, RGBColor{ 0xb4, 0xff, 0x00 } );

    // Boussole : 4 cadrans de 9x9 dans un atlas 36x9, un par direction
    // (meme feuille que l'original, UI_compass.png).
    int frame = (int)facing - 1; // FacingDirection::North=1 -> cadran 0
    if ( frame < 0 || frame > 3 ) frame = 0;
    renderer.drawImageRegionScaled( 87 * 2, 71 * 2, 9 * 2, 9 * 2,
                                     (ImageId)ImageAsset::UICompass, (int16_t)( frame * 9 ), 0, 9, 9 );
}

ImageAsset UI::spriteSheetFor( EnemyType type ) const {
    switch ( type ) {
        case EnemyType::Rat:      return ImageAsset::EnemyRatSpriteSheet;
        case EnemyType::Skeleton: return ImageAsset::EnemySkeletonSpriteSheet;
    }
    return ImageAsset::EnemyRatSpriteSheet;
}

void UI::renderCombat( IRenderer& renderer, const Enemy& enemy ) {
    // Barre de vie de l'ennemi -- meme cadre + remplissage proportionnel
    // que l'original (displayEnemyBar()).
    renderer.drawImageScaled( 30 * 2, 2 * 2, 38 * 2, 12 * 2, (ImageId)ImageAsset::EnemyHPBar );
    float hpRatio = enemy.getCurrentHP() / enemy.getMaxHP();
    renderer.fillRect( 44 * 2, 7 * 2, (int16_t)( hpRatio * 10 * 2 ), 2 * 2, RGBColor{ 0xff, 0x48, 0x00 } );

    switch ( enemy.getFightState() ) {
        case FightState::EnemyAttacking:
            // Frame animee de l'ennemi (3 frames, meme feuille que
            // l'original), puis l'effet de griffure par-dessus.
            renderer.drawImageRegionScaled( 18 * 2, 3 * 2, 64 * 2, 64 * 2,
                                             (ImageId)spriteSheetFor( enemy.getType() ),
                                             (int16_t)( enemy.getAnimFrame() * 64 ), 0, 64, 64 );
            renderer.drawImageScaled( 18 * 2, 3 * 2, 64 * 2, 64 * 2, (ImageId)ImageAsset::EnemyScratch );
            break;

        case FightState::PlayerAttacking:
            renderer.drawImageRegionScaled( 18 * 2, 3 * 2, 64 * 2, 64 * 2,
                                             (ImageId)spriteSheetFor( enemy.getType() ),
                                             (int16_t)( enemy.getAnimFrame() * 64 ), 0, 64, 64 );
            renderer.drawImageScaled( 22 * 2, 4 * 2, 64 * 2, 64 * 2, (ImageId)ImageAsset::UISwordHit );
            break;

        case FightState::EnemyDead:
            renderer.drawImageScaled( 18 * 2, 3 * 2, 64 * 2, 64 * 2, (ImageId)ImageAsset::EnemySplat );
            break;
    }
}
