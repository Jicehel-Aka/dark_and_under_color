// RelativeMovement.h — Dans l'original (Buttons.pde), les boutons du
// D-pad ne sont PAS "haut/bas/gauche/droite" au sens de la grille -- ils
// sont "avancer/reculer/tourner à gauche/tourner à droite" RELATIFS à la
// direction où regarde le joueur (myVision.direction). Une même touche
// "avancer" déplace le joueur vers le nord, l'est, le sud ou l'ouest
// selon qu'il regarde dans cette direction -- table à 4 entrées par
// bouton dans l'original (un switch(direction) par bouton), reprise ici
// telle quelle sous forme de table plutôt que dupliquée dans main.cpp
// des deux plateformes.
#pragma once

#include "rendering/Vision.h" // FacingDirection
#include "entities/Player.h"  // Direction

// Traduit une pression "avancer" (D-pad haut, ou bouton dédié) en
// déplacement absolu selon la direction de vue actuelle.
inline Direction relativeForward( FacingDirection facing ) {
    switch ( facing ) {
        case FacingDirection::North: return Direction::Up;
        case FacingDirection::East:  return Direction::Right;
        case FacingDirection::South: return Direction::Down;
        case FacingDirection::West:  return Direction::Left;
    }
    return Direction::Up;
}
inline Direction relativeBackward( FacingDirection facing ) {
    switch ( facing ) {
        case FacingDirection::North: return Direction::Down;
        case FacingDirection::East:  return Direction::Left;
        case FacingDirection::South: return Direction::Up;
        case FacingDirection::West:  return Direction::Right;
    }
    return Direction::Down;
}
inline Direction relativeStrafeLeft( FacingDirection facing ) {
    switch ( facing ) {
        case FacingDirection::North: return Direction::Left;
        case FacingDirection::East:  return Direction::Up;
        case FacingDirection::South: return Direction::Right;
        case FacingDirection::West:  return Direction::Down;
    }
    return Direction::Left;
}
inline Direction relativeStrafeRight( FacingDirection facing ) {
    switch ( facing ) {
        case FacingDirection::North: return Direction::Right;
        case FacingDirection::East:  return Direction::Down;
        case FacingDirection::South: return Direction::Left;
        case FacingDirection::West:  return Direction::Up;
    }
    return Direction::Right;
}
