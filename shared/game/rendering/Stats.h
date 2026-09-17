// Stats.h — Porté depuis Stats.pde. Écran simple : portrait, PV/XP/AP/DF
// du héros. Le nom "CERYS" et les couleurs de texte sont ceux de
// l'original.
#pragma once

#include "IRenderer.h"
#include "ITranslator.h"
#include "AssetIds.h"

class Player;

class Stats {
  public:
    void render( IRenderer& renderer, const Player& player, ITranslator& translator );
};
