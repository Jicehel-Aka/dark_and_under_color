// IInput.h — Interface d'entrée, commune aux deux cibles. Côté AKA,
// implémentée par-dessus core/input.h (Keys, input_poll()) ; côté SDL,
// par-dessus SDL_PollEvent()/le clavier. Le jeu ne lit jamais les
// touches directement, seulement cette interface.
#pragma once

struct InputState {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool actionA = false; // valider / attaquer / utiliser
    bool actionB = false; // retour / annuler / jeter
    bool actionC = false; // ouvrir l'inventaire
    bool actionD = false; // ouvrir la mini-carte (bascule)
    bool l1 = false;      // tourner a gauche (exploration) / onglet precedent (inventaire)
    bool r1 = false;      // tourner a droite (exploration) / onglet suivant (inventaire)
};

class IInput {
  public:
    virtual ~IInput() = default;

    // Rafraîchit l'état interne -- à appeler une fois par frame, avant
    // toute lecture.
    virtual void poll() = 0;

    virtual const InputState& state() const = 0;

    // true seulement sur la frame où le bouton vient d'être pressé
    // (équivalent justPressed -- utile pour les déplacements case par
    // case, qui ne doivent pas répéter tant que la touche reste
    // enfoncée).
    virtual bool justPressed( bool InputState::*button ) const = 0;
};
