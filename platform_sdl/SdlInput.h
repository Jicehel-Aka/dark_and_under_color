// SdlInput.h/.cpp — Implémentation de IInput pour la build PC : clavier
// (fleches deplacement/strafe, Q/E tourner, Z=A, X=B, C=inventaire,
// V=mini-carte -- voir GameApp/RelativeMovement.h pour la logique qui
// consomme ces boutons). Expose aussi la SOURIS (mouseX/mouseY/
// mouseClickedThisFrame), specifique PC, volontairement HORS de
// l'interface IInput commune (AKA n'a pas de souris) -- utilisee par la
// barre de menu de SdlRenderer (zoom, panneau d'aide), pas par le jeu
// lui-meme.
#pragma once

#include "IInput.h"
#include <SDL2/SDL.h>

class SdlInput : public IInput {
  public:
    void poll() override;
    const InputState& state() const override { return current; }
    bool justPressed( bool InputState::*button ) const override;

    // true si l'utilisateur a demande a fermer la fenetre -- pas dans
    // IInput (specifique desktop, sans equivalent AKA).
    bool quitRequested() const { return quit; }

    // Souris -- specifique PC (menu de zoom en haut de fenetre, voir
    // main.cpp), pas dans IInput non plus : AKA n'a pas de souris, ca
    // n'a rien a faire dans l'interface commune au jeu.
    int mouseX() const { return mouseXPos; }
    int mouseY() const { return mouseYPos; }
    bool mouseClickedThisFrame() const { return mouseClicked; }

  private:
    InputState current;
    InputState previous;
    bool quit = false;
    int mouseXPos = 0, mouseYPos = 0;
    bool mouseClicked = false;
};
