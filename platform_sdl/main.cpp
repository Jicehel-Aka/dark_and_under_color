// main.cpp (build PC/SDL) — Utilise GameApp, le même code que la build
// AKA. Flèches pour se déplacer/strafe, L1(Q)/R1(E) pour tourner,
// Z=A, X=B, C=inventaire, V=mini-carte.
#include "SdlRenderer.h"
#include "SdlInput.h"
#include "SdlTranslator.h"
#include "GameApp.h"
#include "levels/Level.h"

#include <SDL2/SDL_mixer.h>

int main( int argc, char** argv ) {
    constexpr int kWindowW = 150 * 2;
    constexpr int kWindowH = 81 * 2;

    SdlRenderer renderer( kWindowW, kWindowH, "data" );
    if ( !renderer.ok() ) return 1;

    SdlInput input;
    // Langue fixee au francais pour cette build de test -- pas de menu
    // systeme AKA ici pour la changer a la volee. Remplacer "fr" par
    // "en" pour tester l'autre langue disponible.
    SdlTranslator translator( "lang/aka_common_fr.json", "lang/fr.json" );
    GameApp app( kLevel00, translator );

    // Musique -- meme fichier .wav que la build AKA (converti depuis le
    // MP3 d'origine, "Pyramid Level" par Visager, CC BY 4.0 -- voir
    // LICENSE), place directement a cote de l'executable plutot que
    // sous sdcard_files/ (qui n'a de sens que sur AKA). Mix_PlayMusic()
    // boucle nativement (parametre -1), pas besoin du bricolage manuel
    // de redemarrage utilise cote AKA (gb_audio_track_wav n'a pas cette
    // option). Musique desactivee (pas bloquante) si Mix_OpenAudio()
    // echoue -- ex. pas de peripherique son disponible en CI/tests.
    bool audioOk = ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 1, 2048 ) == 0 );
    Mix_Music* music = audioOk ? Mix_LoadMUS( "music/level_loop.wav" ) : nullptr;
    if ( music ) Mix_PlayMusic( music, -1 );

    bool running = true;
    while ( running ) {
        input.poll();
        if ( input.quitRequested() ) running = false;

        app.update( input );
        app.render( renderer );

        SDL_Delay( 16 );
    }

    if ( music ) Mix_FreeMusic( music );
    if ( audioOk ) Mix_CloseAudio();

    return 0;
}
