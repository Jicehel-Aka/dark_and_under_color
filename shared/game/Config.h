// Config.h — Constantes de jeu partagées entre toutes les entités.
// Factorise le TODO laissé dans Object.cpp/Player.cpp (TILE_SIZE dupliqué
// localement dans chaque fichier).
#pragma once

// Taille logique d'une case, AVANT le zoom plateforme (x2 sur AKA --
// voir platform_aka/, zoom configurable sur SDL).
//
// BUG TROUVE ET CORRIGE (debordement de la mini-carte sur le
// parchemin) : la valeur precedente (16) prenait "TILE_SIZE = 4 * multi"
// de l'original pour une taille deja logique -- mais "multi" (=4 sur la
// cible PC de reference, size(150*multi, 81*multi)) EST le facteur
// d'echelle de l'original, deja applique dans ce 16. La vraie taille
// logique (avant tout zoom, la meme echelle que "150x81" utilisee
// partout ailleurs dans ce portage) est 4 px, pas 16 -- verifie
// directement dans darkUnderCOLOR.pde ("int TILE_SIZE = 4*multi;") et
// Player.pde. Avec 16, chaque case de la mini-carte (16*2=32 px) et son
// deplacement associe etaient 4x trop grands, d'ou le dessin qui
// debordait largement du parchemin (9 cases x 32px = 288px de haut,
// contre 162px de parchemin disponible).
constexpr int kTileSize = 4;

// Degats de combat -- UNE SEULE definition, partagee entre l'effet reel
// (Enemy::applyPlayerHit, Player::applyDamage) ET le texte affiche
// (GameApp::updateCombat) -- l'ancien "4DMG" fige en dur dans le texte
// (repris fidelement de l'original, corrige a la demande de Jicehel :
// "vraiment ambigu") venait exactement de ce genre de duplication non
// synchronisee. En centralisant ici, un futur changement d'equilibrage
// ne peut plus faire diverger texte et effet reel.
constexpr int kEnemyDamageToPlayer = 1;
constexpr int kPlayerDamageToEnemy = 2;

// Dimensions de la grille de niveau (20x20 dans l'unique niveau fourni
// pour l'instant -- prevu pour varier si des niveaux de tailles
// differentes sont ajoutes plus tard, voir LevelData.h).
