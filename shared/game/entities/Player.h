// Player.h — Le héros : position, statistiques, déplacement.
//
// Porté depuis Player.pde (voir Object.h pour le contexte général du
// portage). Différences volontaires avec l'original :
//
// - L'original s'appuie sur des VARIABLES GLOBALES pour communiquer
//   avec le reste du jeu (pcolX/prowY dupliquent this->colX/rowY dans
//   des globales lues ailleurs ; moveLegal() lit directement
//   myLevel.worldGrid ; movePlayer() modifie myVision.oddStep) -- un
//   style catalogue chez les sketches Processing, mais qui ne tient
//   pas sans un Level/Vision déjà portés. Remplacé ici par deux
//   callbacks injectés (isTileWalkable, onStepToggled) : Player reste
//   autonome et testable sans dépendre du reste du jeu -- ce découplage
//   est le choix d'architecture qui a permis de porter chaque classe
//   indépendamment, avant que Level/Vision existent encore.
//   Le comportement de jeu est identique, seule la plomberie change.
// - BUG CORRIGE (pas reproduit) : le constructeur d'origine faisait
//   "pcolX=col; prowY=row;" -- PAS "pcolX=colX; prowY=rowY;" (les
//   PARAMETRES du constructeur) -- ça ne marchait que parce que
//   Level.pde assigne les globales row/col juste avant d'appeler
//   "new Player(row, col)". Ici, on utilise directement les paramètres
//   reçus, un choix strictement équivalent dans tous les cas d'usage
//   réels observés, sans dépendre de cet ordre d'exécution fragile.
// - Le rendu (display() dans l'original : juste un rectangle plein
//   écran de debug, jamais visible en jeu puisque la vue à la première
//   personne ne montre pas le héros) n'a pas d'équivalent ici --
//   rien à porter, ce n'était pas utilisé.
#pragma once

#include <functional>

enum class Direction : char { Up = 'U', Down = 'D', Left = 'L', Right = 'R' };

class Player {
  public:
    // isTileWalkable(row, col) : doit renvoyer true si la case est
    // praticable (equivalent de "myLevel.worldGrid[row][col] < 1").
    // onStepToggled() : appele a chaque deplacement reussi (equivalent
    // de "myVision.oddStep = !myVision.oddStep", utilise cote rendu
    // pour l'animation de pas -- Player n'a pas besoin de connaitre
    // Vision, juste de signaler qu'un pas a eu lieu).
    using TileWalkableFn = std::function<bool( int row, int col )>;
    using StepToggledFn  = std::function<void()>;

    Player( int startRow, int startCol, TileWalkableFn isTileWalkable, StepToggledFn onStepToggled );

    // Tente un deplacement d'une case dans la direction donnee.
    // Revient en arriere si la case visee n'est pas praticable --
    // meme logique que l'original (row/col modifies puis annules).
    void movePlayer( Direction direction );

    int getRow() const { return rowY; }
    int getCol() const { return colX; }
    int getPixelX() const { return playerX; }
    int getPixelY() const { return playerY; }

    float getCurrentHP() const { return currentHP; }
    float getMaxHP() const { return maxHP; }
    float getCurrentXP() const { return currentXP; }
    float getMaxXP() const { return maxXP; }
    float getAP() const { return attackPower; }
    float getDF() const { return defense; }

    // Ajoutes pour GameApp (combat/inventaire) -- l'original modifie
    // myHero.currentHP/currentXP directement en tant que champs publics
    // partout ; ici on passe par des methodes explicites, plus facile a
    // tracer/tester, meme effet.
    void applyDamage( float amount ) {
        currentHP -= amount;
        if ( currentHP < 0 ) currentHP = 0;
    }
    void heal( float amount ) {
        currentHP += amount;
        if ( currentHP > maxHP ) currentHP = maxHP;
    }
    void gainXP( float amount ) {
        currentXP += amount;
        if ( currentXP > maxXP ) currentXP = maxXP;
    }

  private:
    TileWalkableFn isTileWalkable;
    StepToggledFn  onStepToggled;

    int playerX;
    int playerY;
    int rowY;
    int colX;

    // Statistiques -- memes valeurs de depart que l'original
    // (maxHP=40, currentXP de depart=10, AP/DF=1).
    float maxHP = 40;
    float currentHP = maxHP;
    float maxXP = 50;
    float currentXP = 10;
    float attackPower = 1;
    float defense = 1;
};
