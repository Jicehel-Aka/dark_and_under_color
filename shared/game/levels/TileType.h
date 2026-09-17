// TileType.h — Les cases de la grille de niveau.
//
// CONFIRME en portant Vision.pde (la valeur 2 n'était pas encore
// clarifiée au moment de porter Level -- voir l'historique) : Special
// (2) n'est PAS une porte ni une sortie de niveau, c'est simplement une
// VARIANTE DE TEXTURE DE MUR alternative, purement décorative -- 8 des
// 11 sprites de mur du couloir existent en 2 versions (normale pour
// Wall=1, variante "_2" pour Special=2), voir AssetIds.h. La case reste
// bloquante pour le déplacement dans les deux cas (Player.pde teste
// "worldGrid[row][col] < 1", donc 1 et 2 bloquent tous les deux),
// exactement comme un mur normal -- seul l'aspect visuel change.
#pragma once

#include <cstdint>

enum class TileType : uint8_t {
    Floor   = 0,
    Wall    = 1,
    Special = 2, // variante de texture de mur, toujours bloquante -- voir ci-dessus
};

// Praticable : reproduit exactement "worldGrid[row][col] < 1" de
// l'original -- seul Floor (0) est franchissable, Wall ET Special
// bloquent tous les deux le déplacement.
inline bool isWalkable( TileType tile ) {
    return tile == TileType::Floor;
}
