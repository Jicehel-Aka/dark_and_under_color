// SdlInput.h/.cpp — Implémentation clavier de IInput pour la build PC.
// Flèches + Z/X (ou A/B au choix) -- juste de quoi tester le
// déplacement pour l'instant, pas encore de vraie configuration de
// touches (viendra avec le portage de l'écran Options).
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

  private:
    InputState current;
    InputState previous;
    bool quit = false;
};
