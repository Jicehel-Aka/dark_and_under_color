// Enemy.h — Ennemi rencontré en combat (rat, squelette pour l'instant).
//
// Porté depuis Enemy.pde (voir Object.h pour le contexte général).
// Différences volontaires :
// - fightSequence (String comparé par valeur : "enemyAttack",
//   "playerAttack", "enemyDead") devient un enum FightState -- même
//   logique, plus sûr et moins coûteux.
// - L'original pilote le combat via mousePressed (clic souris) --
//   remplacé par un bouton d'action générique (actionPressed, déjà
//   InputState::actionA côté appelant), cohérent avec un jeu au clavier/
//   D-pad plutôt qu'à la souris. C'est le changement d'interaction le
//   plus visible entre la version PC/Android tactile et ce portage.
// - La mort d'un ennemi déclenchait directement une Dialogue box et une
//   suppression de la liste globale myEnemies -- ici, Enemy expose juste
//   isDefeated() ; retirer l'ennemi de la liste et déclencher un
//   dialogue est la responsabilité de l'appelant (Level ou un futur
//   gestionnaire de combat), pas d'Enemy lui-même -- même principe de
//   découplage que pour Player/moveLegal.
// - BUG REPÉRÉ, PAS CORRIGÉ (décision de conception, pas à moi de la
//   prendre) : le texte affiché dit "4DMG" des deux côtés, mais les
//   dégâts réels appliqués sont -1 PV pour le joueur et -2 PV pour
//   l'ennemi -- incohérence déjà présente dans l'original. Signalé ici,
//   valeurs conservées telles quelles.
#pragma once

#include <cstdint>
#include <functional>

enum class EnemyType : char { Rat = 'R', Skeleton = 'S' };

enum class FightState {
    EnemyAttacking,
    PlayerAttacking,
    EnemyDead,
};

class Enemy {
  public:
    Enemy( int colX, int rowY, EnemyType type, const char* id );

    // À appeler une fois par frame pendant un combat contre cet ennemi.
    // actionPressed : équivalent de mousePressed dans l'original (bouton
    // d'action générique, pas spécifiquement la souris).
    void update( bool actionPressed );

    bool checkCollision( int checkCol, int checkRow, int playerCol, int playerRow ) const;

    int getCol() const { return enemyCol; }
    int getRow() const { return enemyRow; }
    const char* getId() const { return id; }
    const char* getDescription() const { return description; }
    EnemyType getType() const { return type; }

    float getCurrentHP() const { return currentHP; }
    float getMaxHP() const { return maxHP; }
    int getAnimFrame() const { return animFrame; } // 0..2, index dans la feuille de sprites 3 frames

    FightState getFightState() const { return fightState; }
    bool isDefeated() const { return fightState == FightState::EnemyDead; }

    // Reçoit un coup du joueur -- appelé en interne par update(), mais
    // aussi utilisable directement si le combat est piloté autrement
    // (utile pour les tests).
    void applyPlayerHit( float damage );

  private:
    int enemyX;
    int enemyY;
    int enemyRow;
    int enemyCol;
    EnemyType type;
    const char* id;
    const char* description;

    float maxHP;
    float currentHP;

    FightState fightState = FightState::EnemyAttacking;
    int animFrame = 0;
    bool playerHitAppliedThisBout = false; // equivalent "doOnce" de l'original

    uint32_t lastActionMs = 0; // porte 300ms (timer() dans l'original)
    uint32_t lastAnimMs = 0;   // porte 300ms (animTimer())
};
