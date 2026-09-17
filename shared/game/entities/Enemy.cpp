#include "Enemy.h"
#include "../Config.h"
#include "../../platform/Time.h"

namespace {
constexpr uint32_t kActionCooldownMs = 300; // timer() dans l'original
constexpr uint32_t kAnimCooldownMs   = 300; // animTimer()
}

Enemy::Enemy( int colX, int rowY, EnemyType enemyType, const char* enemyId )
    : enemyRow( rowY )
    , enemyCol( colX )
    , type( enemyType )
    , id( enemyId )
{
    enemyX = colX * kTileSize;
    enemyY = rowY * kTileSize;

    switch ( type ) {
        case EnemyType::Rat:
            maxHP = 20;
            description = "RAT";
            break;
        case EnemyType::Skeleton:
            maxHP = 40;
            description = "SKEL";
            break;
    }
    currentHP = maxHP;
}

bool Enemy::checkCollision( int checkCol, int checkRow, int playerCol, int playerRow ) const {
    return ( checkCol == playerCol ) && ( checkRow == playerRow );
}

void Enemy::applyPlayerHit( float damage ) {
    currentHP -= damage;
    if ( currentHP <= 0 ) fightState = FightState::EnemyDead;
}

void Enemy::update( bool actionPressed ) {
    const uint32_t now = platformMillis();

    // Animation de la feuille de sprites (3 frames), independante du
    // tour de combat -- meme rythme que l'original (animTimer()).
    if ( now - lastAnimMs > kAnimCooldownMs ) {
        lastAnimMs = now;
        animFrame = ( animFrame + 1 ) % 3;
    }

    switch ( fightState ) {
        case FightState::EnemyAttacking:
            if ( currentHP <= 0 ) {
                fightState = FightState::EnemyDead;
                break;
            }
            // Degat inflige une seule fois par assaut (doOnce dans
            // l'original) -- remis a zero au passage a PlayerAttacking.
            if ( !playerHitAppliedThisBout ) {
                playerHitAppliedThisBout = true;
                // NOTE : c'est ICI que l'original decremente
                // myHero.currentHP -- Enemy n'a pas de reference au
                // joueur (meme decouplage que partout ailleurs dans ce
                // portage), donc cet effet doit etre applique par
                // l'appelant en reaction a l'entree dans EnemyAttacking,
                // pas ici.
            }
            if ( actionPressed && ( now - lastActionMs > kActionCooldownMs ) ) {
                lastActionMs = now;
                playerHitAppliedThisBout = false;
                fightState = FightState::PlayerAttacking;
            }
            break;

        case FightState::PlayerAttacking:
            if ( actionPressed && ( now - lastActionMs > kActionCooldownMs ) ) {
                lastActionMs = now;
                applyPlayerHit( 2 ); // voir note "4DMG" dans Enemy.h
                fightState = FightState::EnemyAttacking;
            }
            break;

        case FightState::EnemyDead:
            // Rien a faire ici -- l'appelant retire l'ennemi de sa
            // liste et declenche le dialogue de victoire (voir Enemy.h).
            break;
    }
}
