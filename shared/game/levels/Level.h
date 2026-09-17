// Level.h — Enveloppe runtime autour d'un LevelData : c'est elle que le
// jeu manipule au jour le jour (accès à la grille courante, fabrique le
// callback isTileWalkable attendu par Player).
//
// Amélioration demandée (plus paramétrable) : Level ne connaît qu'UN
// LevelData à la fois, mais n'importe quel LevelData -- passer à un
// niveau différent, c'est juste construire un autre Level avec une
// autre table (voir Level01.cpp le jour où un 2e niveau existera),
// aucune modification de cette classe.
#pragma once

#include "LevelData.h"

class Level {
  public:
    explicit Level( const LevelData& data ) : data( data ) {}

    const LevelData& getData() const { return data; }

    bool isTileWalkable( int row, int col ) const {
        return ::isWalkable( data.tileAt( row, col ) );
    }

    const char* getDescription() const { return data.description; }
    int getStartRow() const { return data.startRow; }
    int getStartCol() const { return data.startCol; }

  private:
    const LevelData& data;
};

// Le seul niveau existant pour l'instant (voir Level00.cpp) -- une fois
// Vision/UI portés et un vrai système de progression en place, cette
// liste s'étendra (kLevel01, kLevel02...) exactement comme
// levels[]/MAX_LEVEL_COUNT sur Dark & Under 1.
extern const LevelData kLevel00;
