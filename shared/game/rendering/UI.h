// UI.h — Porté depuis UI.pde. L'original mélangeait l'affichage
// permanent (barres PV/XP, boussole) ET des boutons tactiles cliquables
// (avancer/tourner/inventaire/stats, dessinés comme icônes qu'on
// survole à la souris). Les boutons tactiles n'ont plus lieu d'être :
// de vrais boutons physiques existent (D-pad, L1/R1, C, D — voir
// GameApp). Cette classe ne garde donc que la partie AFFICHAGE :
// cadre, barres PV/XP, boussole, et pendant un combat, le visuel de
// l'ennemi (sprite animé, barre de vie, effets de coup).
#pragma once

#include "IRenderer.h"
#include "AssetIds.h"

class Player;
class Enemy;
enum class FacingDirection : int;
enum class EnemyType : char;

class UI {
  public:
    // BUG TROUVE ET CORRIGE (le cadre UI recouvrait le couloir au lieu
    // de lui servir de fond) : anciennement une seule methode renderHud()
    // appelee APRES Vision::render() -- UIMain (le cadre plein ecran,
    // avec le panneau de droite integre au dessin) se retrouvait donc
    // dessine PAR-DESSUS le couloir. Separe en deux : renderFrame()
    // (juste le cadre/fond, a appeler AVANT Vision) et renderStatus()
    // (barres PV/XP + boussole, a appeler APRES Vision, par-dessus).
    void renderFrame( IRenderer& renderer );
    void renderStatus( IRenderer& renderer, const Player& player, FacingDirection facing );

    // Affichage spécifique au combat : sprite de l'ennemi (animé),
    // barre de vie, effet de coup selon l'état du combat.
    void renderCombat( IRenderer& renderer, const Enemy& enemy );

  private:
    ImageAsset spriteSheetFor( EnemyType type ) const;
};
