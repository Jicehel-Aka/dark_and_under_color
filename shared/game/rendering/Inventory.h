// Inventory.h — Porté depuis Inventory.pde, entièrement réécrit pour
// une interaction bouton (l'original navigue à la souris, survol +
// clic sur chaque icône). Choix de Jicehel : pas de bouton dédié
// restant pour un écran Stats séparé -- Stats devient un second ONGLET
// de cet écran, basculé par L1/R1 (les mêmes boutons qui tournent le
// regard en exploration, réutilisés ici pour changer d'onglet -- sans
// conflit, on ne se déplace pas dans un menu).
#pragma once

#include "IRenderer.h"
#include "ITranslator.h"
#include "AssetIds.h"
#include "Object.h" // ObjectType
#include <vector>
#include <string>

enum class InventoryTab { Items, Stats };

struct InventoryEntry {
    ObjectType type;
    std::string name; // ce qu'affichait "dialogueItem" dans l'original (ex. "KEY")
};

class Player;

class Inventory {
  public:
    void addItem( ObjectType type, const std::string& name );

    // Retire et renvoie l'objet sélectionné (à utiliser ou jeter) --
    // renvoie false si rien n'est sélectionné/l'inventaire est vide.
    bool takeSelected( InventoryEntry& out );

    void selectNext();   // D-pad droite/bas
    void selectPrev();   // D-pad gauche/haut
    void switchTab();    // L1/R1 -- un seul bouton suffit, 2 onglets seulement

    InventoryTab getActiveTab() const { return activeTab; }
    bool isEmpty() const { return entries.empty(); }

    void render( IRenderer& renderer, const Player& player, ITranslator& translator );

  private:
    std::vector<InventoryEntry> entries;
    int selectedIndex = 0;
    InventoryTab activeTab = InventoryTab::Items;

    ImageAsset iconFor( ObjectType type ) const;
};
