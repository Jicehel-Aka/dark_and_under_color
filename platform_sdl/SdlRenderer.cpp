#include "SdlRenderer.h"
#include "Assets.h"
#include "AssetIds.h"
#include <SDL2/SDL_image.h>
#include <cstdio>

// Meme avancement resserre que cote AKA (voir AkaRenderer.cpp) : 5px
// de glyphe + 1px d'espacement, coherence visuelle entre les deux
// plateformes.
static constexpr int16_t kNarrowCharAdvance = 6;

SdlRenderer::SdlRenderer( int windowWidth, int windowHeight, const std::string& assetDir )
    : assetDir( assetDir )
{
    SDL_Init( SDL_INIT_VIDEO );
    IMG_Init( IMG_INIT_PNG );

    window = SDL_CreateWindow(
        "Dark & Under (couleur) -- build PC de test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight, SDL_WINDOW_SHOWN );

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
}

SdlRenderer::~SdlRenderer() {
    for ( auto& [id, tex] : textureCache ) SDL_DestroyTexture( tex );
    if ( renderer ) SDL_DestroyRenderer( renderer );
    if ( window ) SDL_DestroyWindow( window );
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* SdlRenderer::getTexture( ImageId image ) {
    auto it = textureCache.find( image );
    if ( it != textureCache.end() ) return it->second;

    const char* fileName = assetFileName( image );
    if ( fileName == nullptr ) return nullptr;

    std::string path = assetDir + "/" + fileName;
    SDL_Surface* surface = IMG_Load( path.c_str() );
    if ( surface == nullptr ) {
        std::fprintf( stderr, "SdlRenderer: impossible de charger %s\n", path.c_str() );
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, surface );
    // BUG TROUVE ET CORRIGE (meme sortie de session que la transparence
    // cote AKA) : sans ceci, la transparence RGBA du PNG source pouvait
    // ne pas etre respectee au rendu selon le driver SDL utilise --
    // explicite plutot que de compter sur un defaut favorable.
    SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND );
    SDL_FreeSurface( surface );
    textureCache[image] = texture;
    return texture;
}

void SdlRenderer::fillRect( int16_t x, int16_t y, int16_t w, int16_t h, RGBColor color ) {
    SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, 255 );
    SDL_Rect rect{ x, y, w, h };
    SDL_RenderFillRect( renderer, &rect );
}

void SdlRenderer::drawImage( int16_t x, int16_t y, ImageId image ) {
    SDL_Texture* tex = getTexture( image );
    if ( tex == nullptr ) return;
    int w, h;
    SDL_QueryTexture( tex, nullptr, nullptr, &w, &h );
    SDL_Rect dst{ x, y, w, h };
    SDL_RenderCopy( renderer, tex, nullptr, &dst );
}

void SdlRenderer::drawImageScaled( int16_t x, int16_t y, int16_t dst_w, int16_t dst_h, ImageId image ) {
    SDL_Texture* tex = getTexture( image );
    if ( tex == nullptr ) return;
    SDL_Rect dst{ x, y, dst_w, dst_h };
    SDL_RenderCopy( renderer, tex, nullptr, &dst );
}

void SdlRenderer::drawImageRegionScaled( int16_t x, int16_t y, int16_t dst_w, int16_t dst_h,
                                          ImageId atlas, int16_t src_x, int16_t src_y,
                                          int16_t src_w, int16_t src_h ) {
    SDL_Texture* tex = getTexture( atlas );
    if ( tex == nullptr ) return;
    SDL_Rect src{ src_x, src_y, src_w, src_h };
    SDL_Rect dst{ x, y, dst_w, dst_h };
    SDL_RenderCopy( renderer, tex, &src, &dst );
}

void SdlRenderer::drawText( int16_t x, int16_t y, const char* text, RGBColor color, FontSize size ) {
    // Cote SDL, une seule police disponible (5x8) -- le parametre size
    // est accepte pour respecter l'interface commune mais ignore : pas
    // de donnees 8x8 embarquees ici (uniquement utile pour matcher le
    // rendu AKA, cette build reste une build de test, pas un objectif
    // de fidelite pixel-perfect avec le menu systeme AKA qui n'existe
    // pas cote PC de toute facon).
    (void)size;
    // Rectangle-repere remplace par un vrai rendu de glyphes -- reutilise
    // les memes donnees que la build AKA (simple5x8_font.h, police
    // "Simple 5x8", licence 1001Fonts Free For Commercial Use) plutot
    // que d'ajouter une dependance SDL_ttf : coherence visuelle garantie
    // entre les deux plateformes, et rien a installer de plus ici.
    SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, 255 );

    int16_t cx = x;
    for ( const char* p = text; *p != '\0'; ++p ) {
        uint8_t code = (uint8_t)*p;
        if ( code < 32 || code > 126 ) { cx += kNarrowCharAdvance; continue; }
        const uint8_t* glyph = simple5x8_font[code - 32];
        for ( uint8_t dy = 0; dy < kSimple5x8Height; ++dy ) {
            uint8_t line = glyph[dy];
            for ( uint8_t dx = 0; dx < kSimple5x8Width; ++dx ) {
                if ( line & 1 ) SDL_RenderDrawPoint( renderer, cx + dx, y + dy );
                line >>= 1;
            }
        }
        cx += kNarrowCharAdvance;
    }
}

void SdlRenderer::getImageSize( ImageId image, int16_t& outW, int16_t& outH ) const {
    auto it = textureCache.find( image );
    if ( it == textureCache.end() ) { outW = outH = 0; return; }
    int w, h;
    SDL_QueryTexture( it->second, nullptr, nullptr, &w, &h );
    outW = (int16_t)w;
    outH = (int16_t)h;
}

void SdlRenderer::present() {
    SDL_RenderPresent( renderer );
}

bool SdlRenderer::verifyAssetsLoadable() {
    // ObjPotionIcon (le tout premier asset de l'enum, voir AssetIds.h) --
    // fait partie des 12 tout premiers assets ajoutes au tout debut du
    // portage, garanti present quel que soit l'etat du reste de la
    // table -- si celui-la charge, le dossier data/ est bon.
    SDL_Texture* tex = getTexture( (ImageId)ImageAsset::ObjPotionIcon );
    if ( tex != nullptr ) return true;

    std::string msg = "Impossible de charger les images du jeu depuis :\n" + assetDir +
        "\n\nVerifie que le dossier \"data\" se trouve bien a cote de l'executable.";
    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Dark & Under (couleur) -- assets introuvables", msg.c_str(), window );
    return false;
}
