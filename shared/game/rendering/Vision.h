// Vision.h — Le rendu du couloir en pseudo-3D : c'est le cœur visuel du
// jeu. Porté depuis Vision.pde, avec une réécriture structurelle
// importante (demandée : "plus propre") :
//
// L'original a QUATRE fonctions quasi identiques (lookForward, lookRight,
// lookBack, lookLeft), une par direction du joueur, chacune vérifiant
// les 11 mêmes cases avec des décalages row/col différents -- environ
// 350 lignes de code dupliqué 4 fois. Les 11 décalages suivent en
// réalité un motif géométrique simple une fois qu'on raisonne en
// "devant/à droite" plutôt qu'en "nord/sud/est/ouest" : chaque case
// vérifiée est à (forward × vecteur_avant + right × vecteur_droite) de
// la position du joueur, où vecteur_droite = rotation à 90° du
// vecteur_avant. Vérifié mathématiquement, case par case, contre les 4
// fonctions d'origine avant d'être remplacé -- même résultat, une seule
// fonction générique (~13 lignes de table + une boucle) au lieu de 4
// copies. Voir Vision.cpp pour la table et la démonstration en
// commentaire.
#pragma once

#include "IRenderer.h"
#include "AssetIds.h"
#include <vector>

class Level;
class Player;
class GameObject;
class Enemy;

enum class FacingDirection : int { North = 1, East = 2, South = 3, West = 4 };

// Avance/recule la direction en bouclant 1..4 (equivalent de
// "direction++"/"direction--" avec les clauses ">4"/"<1" de l'original).
FacingDirection rotateRight( FacingDirection d );
FacingDirection rotateLeft( FacingDirection d );

class Vision {
  public:
    // oddStep bascule a chaque pas reussi (voir Player -- callback
    // onStepToggled) : purement decoratif, alterne entre les 2 images de
    // fond du couloir pour un leger effet de balancement.
    void setOddStep( bool value ) { oddStep = value; }

    FacingDirection getDirection() const { return direction; }
    void setDirection( FacingDirection d ) { direction = d; }
    bool getOddStep() const { return oddStep; }

    bool collisionEnabled = true; // desactive temporairement par Dialogue (voir plus tard)

    // Dessine le couloir vu depuis la position/direction du joueur :
    // fond, murs (jusqu'a 3 cases devant), objets et ennemis visibles.
    void render( IRenderer& renderer, const Level& level, const Player& player,
                 const std::vector<GameObject>& objects, const std::vector<Enemy>& enemies );

  private:
    bool oddStep = false;
    FacingDirection direction = FacingDirection::North;

    void renderWalls( IRenderer& renderer, const Level& level, int playerRow, int playerCol );
    void renderObjects( IRenderer& renderer, const Level& level, int playerRow, int playerCol,
                         const std::vector<GameObject>& objects );
    void renderEnemies( IRenderer& renderer, const Level& level, int playerRow, int playerCol,
                         const std::vector<Enemy>& enemies );
};
