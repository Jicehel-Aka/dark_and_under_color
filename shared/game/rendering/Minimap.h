// Minimap.h — Porté depuis Minimap.pde. Fenêtre 9x9 autour du joueur
// (comme l'original : prowY-4..prowY+4, pcolX-4..pcolX+4), marqueur du
// joueur orienté selon la direction de vue, objets/ennemis visibles
// seulement à 4 cases ou moins.
#pragma once

#include "IRenderer.h"
#include "rendering/Vision.h" // FacingDirection
#include <vector>

class Level;
class Player;
class GameObject;
class Enemy;

class Minimap {
  public:
    void render( IRenderer& renderer, const Level& level, const Player& player,
                 FacingDirection facing,
                 const std::vector<GameObject>& objects, const std::vector<Enemy>& enemies );
};
