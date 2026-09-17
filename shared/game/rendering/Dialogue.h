// Dialogue.h — Porté depuis Dialogue.pde. Deux états : Loot (objet
// trouvé, A=accepter/ramasser, B=refuser) et WinFight (ennemi vaincu,
// A=continuer). Mouse over remplacé par les deux boutons A/B fixes --
// plus besoin de savoir où pointe une souris qui n'existe pas ici.
#pragma once

#include "IRenderer.h"
#include "ITranslator.h"
#include "AssetIds.h"
#include "Object.h" // ObjectType
#include <string>

enum class DialogueKind { Loot, WinFight, None };

class Dialogue {
  public:
    void showLoot( ObjectType type, const std::string& itemName );
    void showWinFight( const std::string& enemyName, int xpAwarded );
    void close() { kind = DialogueKind::None; }

    DialogueKind getKind() const { return kind; }
    ObjectType getLootType() const { return lootType; }
    const std::string& getItemName() const { return itemName; }
    int getXpAwarded() const { return xpAwarded; }

    void render( IRenderer& renderer, ITranslator& translator );

  private:
    DialogueKind kind = DialogueKind::None;
    ObjectType lootType = ObjectType::Potion;
    std::string itemName;
    int xpAwarded = 0;

    ImageAsset lootIconFor( ObjectType type ) const;
};
