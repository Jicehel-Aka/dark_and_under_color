// Splash.cpp — Implémentation, voir Splash.h pour la machine à états
// complète et l'historique du bug A/B corrigé.
#include "Splash.h"

void Splash::onButtonA() {
    switch ( state ) {
        case SplashState::Credits: state = SplashState::Title; break;
        case SplashState::Title:   state = SplashState::Intro; break; // bouton PLAY
        case SplashState::Intro:   state = SplashState::Done;  break;
        case SplashState::Done:    break;
    }
}

void Splash::onButtonB() {
    if ( state == SplashState::Title ) state = SplashState::Credits; // bouton CREDITS
}

void Splash::render( IRenderer& renderer, ITranslator& translator ) {
    switch ( state ) {
        case SplashState::Credits:
            renderer.drawImageScaled( 0, 0, 150 * 2, 81 * 2, (ImageId)ImageAsset::SplashCredits );
            break;

        case SplashState::Title:
            renderer.drawImageScaled( 0, 0, 150 * 2, 81 * 2, (ImageId)ImageAsset::SplashTitleScreen );
            renderer.drawText( 20 * 2, 66 * 2, translator.translate( "SPLASH_PLAY" ), RGBColor{ 0x19, 0x18, 0x14 } );
            renderer.drawText( 80 * 2, 66 * 2, translator.translate( "SPLASH_CREDITS" ), RGBColor{ 0x19, 0x18, 0x14 } );
            break;

        case SplashState::Intro:
            renderer.drawImageScaled( 0, 0, 150 * 2, 81 * 2, (ImageId)ImageAsset::SplashIntro );
            renderer.drawText( 28 * 2, 14 * 2, translator.translate( "SPLASH_INTRO_1" ), RGBColor{ 0x19, 0x18, 0x14 } );
            renderer.drawText( 28 * 2, 21 * 2, translator.translate( "SPLASH_INTRO_2" ), RGBColor{ 0x19, 0x18, 0x14 } );
            renderer.drawText( 28 * 2, 28 * 2, translator.translate( "SPLASH_INTRO_3" ), RGBColor{ 0x19, 0x18, 0x14 } );
            renderer.drawText( 28 * 2, 35 * 2, translator.translate( "SPLASH_INTRO_4" ), RGBColor{ 0x19, 0x18, 0x14 } );
            renderer.drawText( 28 * 2, 42 * 2, translator.translate( "SPLASH_INTRO_5" ), RGBColor{ 0x19, 0x18, 0x14 } );
            renderer.drawText( 28 * 2, 49 * 2, translator.translate( "SPLASH_INTRO_6" ), RGBColor{ 0x19, 0x18, 0x14 } );
            break;

        case SplashState::Done:
            break;
    }
}
