// GameApp.h — Le point d'assemblage de tout ce qui a été porté jusqu'ici.
// Équivalent des variables globales + draw() de darkUnderCOLOR.pde,
// mais en une classe possédée par chaque main.cpp plateforme plutôt
// qu'éparpillé en globales -- c'est la seule pièce du portage qui
// "connaît" toutes les autres, exactement comme l'original le faisait
// (juste regroupé en une classe plutôt qu'en variables globales et
// fonctions libres).
#pragma once

#include <string>
#include "IRenderer.h"
#include "IInput.h"
#include "levels/Level.h"
#include "entities/Player.h"
#include "entities/Object.h"
#include "entities/Enemy.h"
#include "rendering/Vision.h"
#include "rendering/Minimap.h"
#include "rendering/Stats.h"
#include "rendering/Splash.h"
#include "rendering/Inventory.h"
#include "rendering/Dialogue.h"
#include "rendering/UI.h"
#include "ITranslator.h"
#include <vector>

enum class ScreenMode { Splash, Exploring, Combat, MinimapView, Inventory, DialogueView };

class GameApp {
  public:
    GameApp( const LevelData& levelData, ITranslator& translator );

    void update( const IInput& input );
    void render( IRenderer& renderer );

  private:
    Level level;
    Player player;
    std::vector<GameObject> objects;
    std::vector<Enemy> enemies;
    Vision vision;
    Minimap minimap;
    Stats stats;
    Splash splash;
    ::Inventory inventory; // "Inventory" seul entre en collision avec ScreenMode::Inventory
    Dialogue dialogue;
    UI ui;
    ITranslator& translator;

    ScreenMode mode = ScreenMode::Splash;
    ScreenMode modeBeforeDialogue = ScreenMode::Exploring; // pour revenir au bon endroit apres un dialogue
    ScreenMode modeBeforeInventory = ScreenMode::Exploring; // BUG TROUVE ET CORRIGE : l'inventaire n'etait accessible qu'en exploration, jamais en combat -- impossible de boire une potion pendant un combat
    int activeEnemyIndex = -1;      // index dans "enemies" pendant un combat, -1 sinon
    int pendingLootObjectIndex = -1; // index dans "objects" en attente de reponse au dialogue de butin
    std::string combatMessage;       // message affiche dans la bande du bas pendant un combat (voir descriptionKey())
    FightState lastSeenFightState = FightState::EnemyAttacking; // BUG TROUVE ET CORRIGE : etait une "static" locale a updateCombat(), donc partagee entre tous les combats au lieu d'etre propre a chacun

    void updateExploring( const IInput& input );
    void updateCombat( const IInput& input );
    void updateInventory( const IInput& input );
    void updateDialogue( const IInput& input );
    void checkTriggers(); // collisions objets/ennemis -> bascule vers Combat/DialogueView
    const char* descriptionKey() const; // texte permanent en bas d'ecran (voir render())
};
