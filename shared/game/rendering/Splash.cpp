// Splash.cpp — Implémentation, voir Splash.h pour la machine à états
// complète et l'historique du bug A/B corrigé.
#include "Splash.h"
#include <cstring>

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

        case SplashState::Title: {
            renderer.drawImageScaled( 0, 0, 150 * 2, 81 * 2, (ImageId)ImageAsset::SplashTitleScreen );
            // BUG TROUVE ET CORRIGE (retour de Jicehel : "grand vide
            // sous Dark & Under") : le texte PLAY/CREDITS etait dessine
            // flottant, en couleur sombre, directement sur le fond deja
            // sombre de l'ecran-titre -- invisible par manque de
            // contraste. L'original les pose sur un vrai bouton
            // graphique clair (BUT_largeBut.png) ; reintegre ici (avait
            // ete retire a tort comme "asset mort" en session 33).
            //
            // 2e BUG TROUVE ET CORRIGE (retour de Jicehel, capture
            // d'ecran a l'appui) : boutons trop etroits (le texte le
            // plus long, "B: CREDITS"/equivalents, depassait d'environ
            // 2 lettres) et texte cale en haut du bouton au lieu d'etre
            // centre verticalement. Boutons elargis (56x22 au lieu de
            // 46x18) et texte desormais centre horizontalement ET
            // verticalement dans chaque bouton, calcule dynamiquement
            // sur la longueur reelle du texte (varie selon la langue).
            constexpr int16_t kButtonW = 56 * 2, kButtonH = 22 * 2;
            constexpr int16_t kGap = 20;
            constexpr int16_t kMarginX = ( 150 * 2 - kButtonW * 2 - kGap ) / 2;
            constexpr int16_t kButtonY = 64 * 2;
            const int16_t playX = kMarginX;
            const int16_t creditsX = kMarginX + kButtonW + kGap;

            renderer.drawImageScaled( playX, kButtonY, kButtonW, kButtonH, (ImageId)ImageAsset::SplashLargeButton );
            renderer.drawImageScaled( creditsX, kButtonY, kButtonW, kButtonH, (ImageId)ImageAsset::SplashLargeButton );

            const char* playText = translator.translate( "SPLASH_PLAY" );
            const char* creditsText = translator.translate( "SPLASH_CREDITS" );
            constexpr int16_t kWideCharWidthPx = 8, kWideCharHeightPx = 8;
            const int16_t textY = (int16_t)( kButtonY + ( kButtonH - kWideCharHeightPx ) / 2 );
            const int16_t playTextX = (int16_t)( playX + ( kButtonW - (int16_t)strlen( playText ) * kWideCharWidthPx ) / 2 );
            const int16_t creditsTextX = (int16_t)( creditsX + ( kButtonW - (int16_t)strlen( creditsText ) * kWideCharWidthPx ) / 2 );

            renderer.drawText( playTextX, textY, playText, RGBColor{ 0x19, 0x18, 0x14 } );
            renderer.drawText( creditsTextX, textY, creditsText, RGBColor{ 0x19, 0x18, 0x14 } );
            break;
        }

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
