// AkaInput.cpp — Implémentation, voir AkaInput.h pour le rôle du
// fichier.
#include "AkaInput.h"

void AkaInput::poll() {
    previous = current;

    Keys keys;
    input_poll( keys );

    current.up      = keys.up;
    current.down    = keys.down;
    current.left    = keys.left;
    current.right   = keys.right;
    current.actionA = keys.A;
    current.actionB = keys.B;
    current.actionC = keys.C;
    current.actionD = keys.D;
    current.l1      = keys.L1;
    current.r1      = keys.R1;
}

bool AkaInput::justPressed( bool InputState::*button ) const {
    return ( current.*button ) && !( previous.*button );
}

#include "PlatformTime.h"
#include "esp_timer.h"
uint32_t platformMillis() {
    return (uint32_t)( esp_timer_get_time() / 1000 );
}
