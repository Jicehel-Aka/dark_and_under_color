#include "SdlInput.h"

void SdlInput::poll() {
    previous = current;

    SDL_Event event;
    while ( SDL_PollEvent( &event ) ) {
        if ( event.type == SDL_QUIT ) quit = true;
    }

    const Uint8* keys = SDL_GetKeyboardState( nullptr );
    current.up      = keys[SDL_SCANCODE_UP]    != 0;
    current.down    = keys[SDL_SCANCODE_DOWN]  != 0;
    current.left    = keys[SDL_SCANCODE_LEFT]  != 0;
    current.right   = keys[SDL_SCANCODE_RIGHT] != 0;
    current.actionA = keys[SDL_SCANCODE_Z]     != 0;
    current.actionB = keys[SDL_SCANCODE_X]     != 0;
    current.actionC = keys[SDL_SCANCODE_C]     != 0;
    current.actionD = keys[SDL_SCANCODE_V]     != 0;
    current.l1      = keys[SDL_SCANCODE_Q]     != 0;
    current.r1      = keys[SDL_SCANCODE_E]     != 0;

    if ( keys[SDL_SCANCODE_ESCAPE] ) quit = true;
}

bool SdlInput::justPressed( bool InputState::*button ) const {
    return ( current.*button ) && !( previous.*button );
}

#include "Time.h"
uint32_t platformMillis() {
    return SDL_GetTicks();
}
