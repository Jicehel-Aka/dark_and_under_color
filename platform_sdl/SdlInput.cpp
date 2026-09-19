// SdlInput.cpp — Implémentation, voir SdlInput.h pour le rôle du
// fichier. Sonde le clavier ET la souris (poll()) dans la même boucle
// d'évènements SDL -- un seul appel SDL_PollEvent par frame, pour ne
// jamais faire concurrence entre deux lecteurs d'évènements (voir la
// note dans SdlInput.h sur pourquoi la souris est ici et pas ailleurs).
#include "SdlInput.h"

void SdlInput::poll() {
    previous = current;
    mouseClicked = false;

    SDL_Event event;
    while ( SDL_PollEvent( &event ) ) {
        if ( event.type == SDL_QUIT ) quit = true;
        if ( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT ) {
            mouseClicked = true;
        }
    }

    int mx, my;
    SDL_GetMouseState( &mx, &my );
    mouseXPos = mx;
    mouseYPos = my;

    const Uint8* keys = SDL_GetKeyboardState( nullptr );
    // Remappage demande par Jicehel : Gauche/Droite tournent le regard
    // (etait le straff avant), L1/R1 straffent (etait tourner avant) --
    // "moins utile de straffer dans ce jeu". Cle Q/E gardees pour L1/R1
    // (meme touche physique qu'avant, seule la SIGNIFICATION change,
    // deja fait cote GameApp.cpp).
    //
    // Alternatives QWERTY/AZERTY ajoutees en plus des fleches -- scancodes
    // SDL (position PHYSIQUE de la touche, PAS le caractere affiche dessus :
    // SDL_SCANCODE_Q designe toujours la meme touche physique, qu'elle
    // affiche "Q" sur un clavier QWERTY ou "A" sur un clavier AZERTY).
    // Jicehel a decrit le schema en repérant les lettres AZERTY -- traduit
    // ici vers les scancodes QWERTY-equivalents correspondant a LA MEME
    // position physique : W(avancer)=Z azerty, A(tourne gauche)=Q azerty,
    // S(reculer)=S azerty, D(tourne droite)=D azerty, Q(straff
    // gauche)=A azerty, E(straff droit)=E azerty -- schema WASD+QE
    // classique de FPS, universel quel que soit le clavier grace aux
    // scancodes.
    current.up      = keys[SDL_SCANCODE_UP]    != 0 || keys[SDL_SCANCODE_W] != 0;
    current.down    = keys[SDL_SCANCODE_DOWN]  != 0 || keys[SDL_SCANCODE_S] != 0;
    current.left    = keys[SDL_SCANCODE_LEFT]  != 0 || keys[SDL_SCANCODE_A] != 0;
    current.right   = keys[SDL_SCANCODE_RIGHT] != 0 || keys[SDL_SCANCODE_D] != 0;
    current.l1      = keys[SDL_SCANCODE_Q]     != 0;
    current.r1      = keys[SDL_SCANCODE_E]     != 0;

    // Boutons d'action -- touches ZXCV (rangee sous WASD, ergonomique)
    // + alternatives Espace/Retour arriere/Suppr/Entree demandees par
    // Jicehel.
    current.actionA = keys[SDL_SCANCODE_Z]     != 0 || keys[SDL_SCANCODE_SPACE]     != 0;
    current.actionB = keys[SDL_SCANCODE_X]     != 0 || keys[SDL_SCANCODE_BACKSPACE] != 0;
    current.actionC = keys[SDL_SCANCODE_C]     != 0 || keys[SDL_SCANCODE_DELETE]    != 0;
    current.actionD = keys[SDL_SCANCODE_V]     != 0 || keys[SDL_SCANCODE_RETURN]    != 0;

    if ( keys[SDL_SCANCODE_ESCAPE] ) quit = true;
}

bool SdlInput::justPressed( bool InputState::*button ) const {
    return ( current.*button ) && !( previous.*button );
}

#include "PlatformTime.h"
uint32_t platformMillis() {
    return SDL_GetTicks();
}
