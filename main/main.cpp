// main.cpp (AKA) — Utilise GameApp, le même code que la build SDL.
#include "AkaRenderer.h"
#include "AkaInput.h"
#include "AkaTranslator.h"
#include "GameApp.h"
#include "levels/Level.h"

#include "gb_core.h"
#include "gb_graphics.h"
#include "gb_audio_player.h"
#include "gb_audio_track_wav.h"
#include "core/input.h"
#include "aka_runtime/aka_runtime.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

gb_core     g_core;
gb_graphics gfx;

// --- Audio ---------------------------------------------------------
// Musique de niveau temporaire ("Pyramid Level" par Visager, CC BY 4.0
// -- voir LICENSE) : lue directement depuis la carte SD (pas de version
// embarquée en flash comme pour les images -- gb_audio_track_wav lit un
// fichier .wav via play_wav(chemin), inutile de la convertir en tableau
// C). Même schéma que le mixage audio de poa_aka : une tâche FreeRTOS
// dédiée appelle g_audio_player.pool() en boucle -- rien ne mélange
// jamais l'audio sans elle.
gb_audio_player   g_audio_player;
gb_audio_track_wav g_music_track;

static const char* const kMusicPath = "/sdcard/DarkUnderColor/music/level_loop.wav";

static void audio_mix_task( void* ) {
    while ( true ) {
        g_audio_player.pool();
        // Relance la piste en boucle -- gb_audio_track_wav n'a pas
        // d'option de bouclage native (voir gb_audio_track_wav.h),
        // donc on redémarre nous-mêmes dès qu'elle s'arrête.
        if ( !g_music_track.is_playing() ) {
            g_music_track.play_wav( kMusicPath );
        }
        vTaskDelay( pdMS_TO_TICKS( 5 ) );
    }
}

// akaRuntime attend une echelle 0-255 pour set_master_volume() -- meme
// conversion que poa_aka. Un seul morceau pour l'instant (pas de SFX
// séparés), donc le volume "musique" du menu système pilote directement
// le volume maître du lecteur.
static void onVolumeChanged( uint8_t musicVol, uint8_t sfxVol ) {
    // Meme mise a l'echelle 0-100 -> 0-255 qu'au demarrage (voir plus
    // bas dans app_main) -- valeur venant du meme menu, meme echelle.
    uint8_t scaled = (uint8_t)( musicVol > 100 ? 255 : ( musicVol * 255 ) / 100 );
    g_audio_player.set_master_volume( scaled );
    (void)sfxVol;
}

extern "C" void app_main( void ) {
    g_core.init();
    input_init();
    akaRuntime.begin( "DarkUnderColor" );
    akaRuntime.setVolumeChangedCallback( onVolumeChanged );

    // Libellés du menu Commandes (aka_runtime) -- les mêmes clés
    // doivent exister dans sdcard_files/DarkUnderColor/lang/*.json.
    static const char* const kControls[] = {
        "CTRL_MOVE", "CTRL_STRAFE", "CTRL_TURN", "CTRL_ACTION", "CTRL_BACK",
        "CTRL_INVENTORY", "CTRL_MINIMAP", nullptr
    };
    akaRuntime.setControlsKeys( kControls );
    akaRuntime.setCredits( "Dark & Under (couleur, AKA)",
                            "Garage Collective / Press Play On Tape",
                            "Licence a confirmer avec l'auteur", "" );

    AkaRenderer renderer( gfx );
    AkaInput input;
    AkaTranslator translator;
    GameApp app( kLevel00, translator );

    g_audio_player.add_track( &g_music_track );
    // BUG TROUVE ET CORRIGE (pas de son du tout au demarrage) :
    // set_master_volume() n'etait appele QUE depuis le callback
    // onVolumeChanged(), qui ne se declenche que si l'utilisateur ouvre
    // le menu et touche au volume -- au demarrage, le volume maitre du
    // lecteur restait a sa valeur par defaut (silence). Corrige en
    // appliquant explicitement le volume deja charge par
    // akaRuntime.begin() (getMusicVolume()).
    //
    // ECHELLE : gb_ll_audio_set_volume() (verifie dans gb_ll_audio.c)
    // attend une valeur 0-255 ("0 = volume max, 116 = volume min/mute"),
    // mais AkaRuntime::getMusicVolume() renvoie 80 par defaut sur une
    // echelle qui semble pensee comme un pourcentage (0-100), pas 0-255
    // -- passee telle quelle, le volume reel serait bien plus faible que
    // prevu (~31% de l'echelle complete). Mise a l'echelle par
    // precaution (x2.55, 0-100 -> 0-255) en attendant confirmation sur
    // materiel reel -- signale a Jicehel comme une hypothese, pas une
    // certitude absolue, n'ayant pas de sortie serie ESP32 pour la
    // verifier directement d'ici.
    uint8_t rawVolume = akaRuntime.getMusicVolume();
    uint8_t scaledVolume = (uint8_t)( rawVolume > 100 ? 255 : ( rawVolume * 255 ) / 100 );
    g_audio_player.set_master_volume( scaledVolume );
    g_music_track.play_wav( kMusicPath );
    xTaskCreatePinnedToCore( audio_mix_task, "AudioMixTask", 4096, nullptr, 5, nullptr, 1 );

    while ( true ) {
        // BUG TROUVE ET CORRIGE : "Keys keys;" etait declare ici SANS
        // jamais appeler input_poll() avant d'etre passe a
        // akaRuntime.update() -- celui-ci lisait donc k.MENU/k.RUN sur
        // de la memoire de pile non initialisee, empechant le menu
        // systeme, la capture d'ecran (appui long MENU) et le retour au
        // loader (RUN+MENU) de fonctionner correctement. Corrige sur le
        // meme modele que dark_and_under_aka (Arduboy2Aka::nextFrame) :
        // sonder les touches AVANT d'appeler update().
        Keys keys;
        input_poll( keys );
        if ( !akaRuntime.update( keys ) ) continue;

        input.poll();
        app.update( input );
        app.render( renderer );
    }
}
