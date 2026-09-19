#include "AkaRenderer.h"
#include "fonts/simple5x8_font.h"
#include "aka_font/gb_text_render.h"

// Centrage : viewport logique 150x81 (x2 = 300x162) sur un ecran AKA
// 320x240 -- marge (320-300)/2=10, (240-162)/2=39. BUG TROUVE ET
// CORRIGE : jusqu'ici tout etait dessine colle en (0,0), jamais centre.
static constexpr int16_t kOffsetX = 10;
static constexpr int16_t kOffsetY = 39;

// Avancement horizontal entre deux caracteres de la police 5x8 :
// 5px de glyphe + 1px d'espacement, pour rester lisible sans gaspiller
// de place (le 8x8 d'origine avance de 8px, sans marge dediee -- la
// police n'a besoin que de 6 ici, 25% plus etroit).
static constexpr int16_t kNarrowCharAdvance = 6;

// Couleur-cle de transparence -- DOIT etre exactement la meme valeur
// que TRANSPARENT_KEY dans tools/convert_assets.py (0xF81F, magenta),
// sans quoi aucun pixel transparent ne correspondrait a l'appel.
static constexpr uint16_t kTransparentColorKey = 0xF81F;

void AkaRenderer::fillRect( int16_t x, int16_t y, int16_t w, int16_t h, RGBColor color ) {
    gfx.setColor( gfx.makeColor( color.r, color.g, color.b ) );
    gfx.fillRect( x + kOffsetX, y + kOffsetY, w, h );
}

void AkaRenderer::drawImage( int16_t x, int16_t y, ImageId image ) {
    const AkaAsset* asset = getAkaAsset( image );
    if ( asset == nullptr ) return;
    // BUG TROUVE ET CORRIGE : appelait la surcharge SANS couleur-cle --
    // chaque pixel "transparent" (magenta dans les donnees converties)
    // etait donc dessine tel quel au lieu d'etre saute, d'ou l'ecran
    // envahi de magenta des qu'un sprite avec zones transparentes
    // (potion, etc.) etait affiche.
    gfx.drawImage( x + kOffsetX, y + kOffsetY, asset->pixels, asset->width, asset->height, kTransparentColorKey );
}

void AkaRenderer::drawImageScaled( int16_t x, int16_t y, int16_t dst_w, int16_t dst_h, ImageId image ) {
    const AkaAsset* asset = getAkaAsset( image );
    if ( asset == nullptr ) return;
    gfx.drawImageScaled( x + kOffsetX, y + kOffsetY, dst_w, dst_h, asset->pixels, asset->width, asset->height, kTransparentColorKey );
}

void AkaRenderer::drawImageRegionScaled( int16_t x, int16_t y, int16_t dst_w, int16_t dst_h,
                                          ImageId atlas, int16_t src_x, int16_t src_y,
                                          int16_t src_w, int16_t src_h ) {
    const AkaAsset* asset = getAkaAsset( atlas );
    if ( asset == nullptr ) return;
    gfx.drawImageScaled( x + kOffsetX, y + kOffsetY, dst_w, dst_h, asset->pixels, asset->width, asset->height,
                          src_x, src_y, src_w, src_h, kTransparentColorKey );
}

void AkaRenderer::drawText( int16_t x, int16_t y, const char* text, RGBColor color, FontSize size ) {
    // Deux polices disponibles depuis le retour de Jicehel apres test
    // reel : Wide (8x8) convient mieux quand il y a peu de texte a
    // afficher (l'ecran de victoire, par exemple -- le 5x8 y "faisait
    // vide"), Narrow (5x8 "Simple 5x8") reste necessaire la ou plus de
    // texte doit tenir (dialogue de butin). Voir Dialogue.cpp pour le
    // choix fait par ecran.
    //
    // BUG TROUVE ET CORRIGE : Wide passait par gfx.print_str(), limite
    // a l'ASCII pur (font8x8_basic) -- tout caractere accentue
    // s'affichait en "?" ou pas du tout. Remplace par le composant
    // aka_font (fourni par Jicehel) : decodage UTF-8 correct + glyphes
    // accentues francais/allemand/espagnol, meme chasse fixe 8px. Le
    // menu systeme AKA (aka_runtime.cpp) continue lui d'utiliser
    // gfx.print_str() directement -- hors de mon controle, seul le
    // texte dessine par LE JEU (via ici) beneficie de ce changement.
    if ( size == FontSize::Wide ) {
        uint16_t pen = gfx.makeColor( color.r, color.g, color.b );
        int16_t cx = x + kOffsetX;
        const int16_t cy = y + kOffsetY;
        gb_text::draw_utf8( cx, cy, text, [&]( int px, int py ) { gfx.drawPixel( px, py, pen ); } );
        return;
    }

    uint16_t pen = gfx.makeColor( color.r, color.g, color.b );
    int16_t cx = x + kOffsetX;
    const int16_t cy = y + kOffsetY;

    for ( const char* p = text; *p != '\0'; ++p ) {
        uint8_t code = (uint8_t)*p;
        if ( code < 32 || code > 126 ) { cx += kNarrowCharAdvance; continue; } // hors de la table generee -- saute plutot que planter
        const uint8_t* glyph = simple5x8_font[code - 32];
        for ( uint8_t dy = 0; dy < kSimple5x8Height; ++dy ) {
            uint8_t line = glyph[dy];
            for ( uint8_t dx = 0; dx < kSimple5x8Width; ++dx ) {
                if ( line & 1 ) gfx.drawPixel( cx + dx, cy + dy, pen );
                line >>= 1;
            }
        }
        cx += kNarrowCharAdvance;
    }
}

void AkaRenderer::getImageSize( ImageId image, int16_t& outW, int16_t& outH ) const {
    const AkaAsset* asset = getAkaAsset( image );
    if ( asset == nullptr ) { outW = outH = 0; return; }
    outW = (int16_t)asset->width;
    outH = (int16_t)asset->height;
}

void AkaRenderer::present() {
    gfx.update();
}
