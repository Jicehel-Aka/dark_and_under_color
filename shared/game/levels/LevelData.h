// LevelData.h — Description PURE DONNÉES d'un niveau : grille, point de
// départ du héros, listes d'objets/ennemis à faire apparaître.
//
// Amélioration par rapport à l'original demandée par Jicehel ("plus
// propre et plus paramétrable") : Level.pde mélangeait données de
// niveau ET chargement d'assets graphiques (13 PImage de murs chargés
// dans son constructeur, jamais utilisés par Level lui-même -- ce sont
// des ressources pour Vision.pde, simplement chargées là par commodité
// Processing). Ici, LevelData ne contient AUCUNE référence graphique :
// juste les données du niveau. Le rendu (Vision, futur portage) ira
// chercher ses images via Assets.h par leur ImageId, indépendamment du
// niveau chargé. Ça permet aussi de définir plusieurs niveaux comme
// simples tables de données (voir Level00.cpp), sans dupliquer 13
// lignes de chargement d'image à chaque nouveau niveau.
#pragma once

#include <cstdint>
#include "TileType.h"

struct ObjectSpawn {
    int col;
    int row;
    char type; // 'P' potion, 'K' clé, 'M' parchemin -- voir Object.h::ObjectType
    const char* id;
};

struct EnemySpawn {
    int col;
    int row;
    char type; // 'R' rat, 'S' squelette -- a confirmer/etendre au portage d'Enemy.pde
    const char* id;
};

struct LevelData {
    const char* description;
    int width;
    int height;
    const TileType* grid; // tableau plat, width*height entrees, [row*width+col]

    int startRow;
    int startCol;

    const ObjectSpawn* objects;
    int objectCount;

    const EnemySpawn* enemies;
    int enemyCount;

    TileType tileAt( int row, int col ) const {
        if ( row < 0 || row >= height || col < 0 || col >= width ) return TileType::Wall;
        return grid[row * width + col];
    }
};
