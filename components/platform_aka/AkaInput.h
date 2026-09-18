// AkaInput.h/.cpp — Implémentation de IInput pour la build AKA : D-pad
// + boutons physiques (A/B/C/D/L1/R1) via core/input.h, meme role que
// SdlInput.h cote PC (pas de souris ici, AKA n'en a pas).
#pragma once

#include "IInput.h"
#include "core/input.h"

class AkaInput : public IInput {
  public:
    void poll() override;
    const InputState& state() const override { return current; }
    bool justPressed( bool InputState::*button ) const override;

  private:
    InputState current;
    InputState previous;
};
