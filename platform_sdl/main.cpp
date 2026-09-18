// main.cpp (build PC/SDL) — Utilise GameApp, le même code que la build
// AKA. Flèches pour se déplacer/strafe, L1(Q)/R1(E) pour tourner,
// Z=A, X=B, C=inventaire, V=mini-carte.
//
// Zoom de fenetre configurable (x2 par defaut) : lancer avec un
// argument 2, 3 ou 4, ex. "darkandundercolor_pc.exe 3" pour un zoom x3.
#include "SdlRenderer.h"
#include "SdlInput.h"
#include "SdlTranslator.h"
#include "GameApp.h"
#include "levels/Level.h"

#include <SDL2/SDL_mixer.h>
#include <string>
#include <cstdlib>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

namespace {
// BUG TROUVE ET CORRIGE (retour de Jicehel : musique OK mais ecran
// noir -- data/ contient ~105 fichiers contre 1 seul pour music/, piste
// probable : chemin trop long ou extraction partielle sous Windows) :
// les chemins ("data", "lang/...", "music/...") etaient relatifs au
// DOSSIER DE TRAVAIL COURANT au lancement, qui ne correspond pas
// toujours au dossier de l'executable selon comment il est demarre
// (double-clic Explorer, raccourci, autre lanceur...). Resolu
// desormais par rapport a l'emplacement REEL de l'executable
// (GetModuleFileNameA sous Windows, /proc/self/exe sous Linux) --
// fiable quel que soit le repertoire de travail au demarrage.
std::string exeDirectory() {
#ifdef _WIN32
    char buf[MAX_PATH];
    DWORD len = GetModuleFileNameA( nullptr, buf, MAX_PATH );
    std::string path( buf, len );
    size_t pos = path.find_last_of( "\\/" );
    return ( pos == std::string::npos ) ? "." : path.substr( 0, pos );
#else
    char buf[PATH_MAX];
    ssize_t len = readlink( "/proc/self/exe", buf, sizeof( buf ) - 1 );
    if ( len <= 0 ) return ".";
    buf[len] = '\0';
    std::string path( buf );
    size_t pos = path.find_last_of( '/' );
    return ( pos == std::string::npos ) ? "." : path.substr( 0, pos );
#endif
}
}

int main( int argc, char** argv ) {
    // Zoom PC configurable (retour de Jicehel) : x2 par defaut, x3/x4
    // au choix via un argument en ligne de commande -- ne change QUE la
    // taille de la fenetre (voir SdlRenderer::SdlRenderer pour le detail
    // de pourquoi l'espace de rendu interne, lui, reste fixe).
    int zoom = 2;
    if ( argc > 1 ) {
        int requested = std::atoi( argv[1] );
        if ( requested >= 2 && requested <= 4 ) zoom = requested;
        else std::fprintf( stderr, "Zoom invalide (%s) -- valeurs acceptees : 2, 3, 4. Zoom x2 applique par defaut.\n", argv[1] );
    }
    const std::string baseDir = exeDirectory();

    SdlRenderer renderer( zoom, baseDir + "/data" );
    if ( !renderer.ok() ) return 1;
    // Verification immediate et VISIBLE (pas juste un message dans une
    // console qui n'existe pas quand on double-clique l'exe) : si le
    // premier asset ne charge pas, le joueur voit exactement pourquoi
    // au lieu d'un ecran noir muet.
    if ( !renderer.verifyAssetsLoadable() ) return 1;

    SdlInput input;
    // Langue fixee au francais pour cette build de test -- pas de menu
    // systeme AKA ici pour la changer a la volee. Remplacer "fr" par
    // "en" pour tester l'autre langue disponible.
    SdlTranslator translator( baseDir + "/lang/aka_common_fr.json", baseDir + "/lang/fr.json" );
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
    Mix_Music* music = audioOk ? Mix_LoadMUS( ( baseDir + "/music/level_loop.wav" ).c_str() ) : nullptr;
    if ( music ) Mix_PlayMusic( music, -1 );

    bool running = true;
    while ( running ) {
        input.poll();
        if ( input.quitRequested() ) running = false;

        app.update( input );

        // Barre de menu demandee par Jicehel : le jeu se dessine dans
        // sa zone habituelle (beginGameArea positionne viewport+echelle
        // en consequence), la barre elle-meme APRES, en coordonnees
        // reelles -- present() n'est plus appele par GameApp::render()
        // (voir GameApp.h) justement pour pouvoir intercaler ce dessin
        // sans decalage d'une frame.
        renderer.beginGameArea();
        app.render( renderer );
        int clickedZoom = renderer.renderMenuBar( input.mouseX(), input.mouseY(), input.mouseClickedThisFrame() );
        if ( clickedZoom != 0 ) renderer.setZoom( clickedZoom );
        if ( renderer.isHelpPanelOpen() ) renderer.renderHelpPanel();
        renderer.present();

        SDL_Delay( 16 );
    }

    if ( music ) Mix_FreeMusic( music );
    if ( audioOk ) Mix_CloseAudio();

    return 0;
}
