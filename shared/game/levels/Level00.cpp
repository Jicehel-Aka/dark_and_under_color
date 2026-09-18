// Level00.cpp — Données du niveau "UNDERGROUND LAIR" (le seul niveau
// fourni dans la version couleur pour l'instant). Grille et listes de
// spawn transcrites depuis Level.pde -- voir LevelData.h pour le
// pourquoi de cette séparation données/rendu.
#include "LevelData.h"

namespace {

constexpr int kWidth = 20;
constexpr int kHeight = 20;

// Grille 20x20 -- valeurs identiques à worldGrid de l'original.
// Note sur TileType::Special (une seule case, ligne 1 dernière colonne) :
// variante décorative de texture de mur, confirmé en portant Vision.pde
// (voir TileType.h) -- pas une porte/sortie comme supposé au départ.
constexpr TileType kGrid[kWidth * kHeight] = {
    TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Special,
    TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Wall,
    TileType::Wall, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Wall,
    TileType::Wall, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Wall, TileType::Floor, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall, TileType::Floor, TileType::Wall,
    TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall, TileType::Wall,
};

constexpr ObjectSpawn kObjects[] = {
    { 3, 3, 'P', "1" },
    { 3, 4, 'M', "2" },
    { 8, 5, 'K', "3" },
    { 8, 15, 'P', "4" },
    { 1, 16, 'P', "5" },
};

constexpr EnemySpawn kEnemies[] = {
    { 8, 3, 'R', "1" },
    { 1, 1, 'R', "2" },
    { 10, 1, 'S', "3" },
    { 3, 15, 'R', "4" },
};

} // namespace

// BUG TROUVE ET CORRIGE (edition de liens, "undefined reference to
// 'kLevel00'") : une variable globale "const" a une liaison INTERNE en
// C++ par defaut (contrairement au C), meme si sa declaration dans un
// en-tete est marquee "extern" (Level.h) -- c'est la DEFINITION
// elle-meme qui doit porter "extern" pour que le symbole soit visible
// depuis un autre fichier .cpp (main.cpp ici). Sans ce mot-cle, chaque
// fichier qui inclut Level.h et utilise kLevel00 croit qu'un symbole
// externe existe, mais l'unique definition reelle (dans CE fichier)
// reste invisible au lien.
extern const LevelData kLevel00 = {
    /* description  */ "UNDERGROUND LAIR",
    /* width        */ kWidth,
    /* height       */ kHeight,
    /* grid         */ kGrid,
    /* startRow     */ 18,
    /* startCol     */ 1,
    /* objects      */ kObjects,
    /* objectCount  */ 5,
    /* enemies      */ kEnemies,
    /* enemyCount   */ 4,
};
