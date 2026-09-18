#include "SdlRenderer.h"
#include "Assets.h"
#include "AssetIds.h"
#include <SDL2/SDL_image.h>
#include <cstdio>

// Meme avancement resserre que cote AKA (voir AkaRenderer.cpp) : 5px
// de glyphe + 1px d'espacement, coherence visuelle entre les deux
// plateformes.
static constexpr int16_t kNarrowCharAdvance = 6;

SdlRenderer::SdlRenderer( int zoom, const std::string& assetDir )
    : assetDir( assetDir ), currentZoom( zoom )
{
    SDL_Init( SDL_INIT_VIDEO );
    IMG_Init( IMG_INIT_PNG );

    window = SDL_CreateWindow(
        "Dark & Under (couleur) -- build PC de test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        150 * zoom, 81 * zoom + kMenuBarHeight, SDL_WINDOW_SHOWN );

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

    // BUG TROUVE ET CORRIGE (2e passe) : SDL_RenderSetLogicalSize (1ere
    // version du zoom configurable) ne cohabite pas proprement avec une
    // barre de menu en haut de fenetre -- son cadrage/letterboxing
    // automatique s'applique a TOUTE la fenetre, pas a une sous-zone.
    // Remplace par un controle manuel (viewport + echelle, voir
    // beginGameArea()) : le jeu se dessine dans la zone sous la barre,
    // la barre elle-meme en coordonnees reelles non affectees par le
    // zoom -- voir renderMenuBar().
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

void SdlRenderer::beginGameArea() {
    // Le jeu dessine dans des coordonnees "300x162" (zoom x2 deja
    // integre dans le code, voir Config.h) -- l'echelle SUPPLEMENTAIRE
    // pour atteindre x3/x4 est zoom/2. Viewport decale sous la barre de
    // menu (kMenuBarHeight, en pixels REELS, non affecte par cette
    // echelle -- c'est le viewport qui la positionne, pas le facteur
    // d'echelle qui s'applique APRES ce decalage).
    SDL_Rect gameViewport{ 0, kMenuBarHeight, 150 * currentZoom, 81 * currentZoom };
    SDL_RenderSetViewport( renderer, &gameViewport );
    float scale = currentZoom / 2.0f;
    SDL_RenderSetScale( renderer, scale, scale );
}

void SdlRenderer::drawMenuText( int x, int y, const char* text, RGBColor color ) {
    SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, 255 );
    int cx = x;
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

int SdlRenderer::renderMenuBar( int mouseX, int mouseY, bool mouseClicked ) {
    // Barre de menu demandee par Jicehel -- coordonnees REELLES (pas
    // affectees par le zoom du jeu, donc toujours la meme taille de
    // texte/boutons quel que soit le zoom choisi). Viewport plein
    // ecran + echelle 1:1 le temps de ce dessin.
    SDL_RenderSetViewport( renderer, nullptr );
    SDL_RenderSetScale( renderer, 1.0f, 1.0f );

    SDL_SetRenderDrawColor( renderer, 30, 28, 24, 255 );
    SDL_Rect barRect{ 0, 0, 150 * currentZoom, kMenuBarHeight };
    SDL_RenderFillRect( renderer, &barRect );

    drawMenuText( 4, 6, "ZOOM:", RGBColor{ 0xcc, 0xc0, 0xa0 } );

    struct ZoomButton { int zoom; int x; };
    const ZoomButton buttons[3] = { { 2, 46 }, { 3, 76 }, { 4, 106 } };
    constexpr int kButtonW = 24, kButtonH = 16;

    int clickedZoom = 0;
    for ( const ZoomButton& b : buttons ) {
        SDL_Rect rect{ b.x, 2, kButtonW, kButtonH };
        bool hovered = ( mouseX >= rect.x && mouseX < rect.x + rect.w && mouseY >= rect.y && mouseY < rect.y + rect.h );
        bool selected = ( b.zoom == currentZoom );

        if ( selected ) SDL_SetRenderDrawColor( renderer, 0xff, 0x48, 0x00, 255 );
        else if ( hovered ) SDL_SetRenderDrawColor( renderer, 70, 65, 55, 255 );
        else SDL_SetRenderDrawColor( renderer, 50, 46, 40, 255 );
        SDL_RenderFillRect( renderer, &rect );

        char label[4];
        std::snprintf( label, sizeof( label ), "x%d", b.zoom );
        drawMenuText( b.x + 4, 6, label, selected ? RGBColor{ 0, 0, 0 } : RGBColor{ 0xe0, 0xd8, 0xc8 } );

        if ( hovered && mouseClicked ) clickedZoom = b.zoom;
    }

    // Bouton "?" -- panneau d'aide (commandes, versions, credits, lien
    // GitHub) demande par Jicehel. Juste a droite des boutons de zoom.
    {
        SDL_Rect rect{ 140, 2, kButtonH, kButtonH }; // carre, meme hauteur que les boutons de zoom
        bool hovered = ( mouseX >= rect.x && mouseX < rect.x + rect.w && mouseY >= rect.y && mouseY < rect.y + rect.h );
        if ( helpPanelOpen ) SDL_SetRenderDrawColor( renderer, 0xff, 0x48, 0x00, 255 );
        else if ( hovered ) SDL_SetRenderDrawColor( renderer, 70, 65, 55, 255 );
        else SDL_SetRenderDrawColor( renderer, 50, 46, 40, 255 );
        SDL_RenderFillRect( renderer, &rect );
        drawMenuText( rect.x + 5, 6, "?", helpPanelOpen ? RGBColor{ 0, 0, 0 } : RGBColor{ 0xe0, 0xd8, 0xc8 } );
        if ( hovered && mouseClicked ) helpPanelOpen = !helpPanelOpen;
    }

    return clickedZoom;
}

void SdlRenderer::renderHelpPanel() {
    // Panneau d'aide demande par Jicehel : touche PC / bouton AKA
    // correspondant + a quoi il sert, autres versions du jeu, credits,
    // lien GitHub. Coordonnees reelles (viewport deja remis a plein
    // ecran + echelle 1:1 par renderMenuBar() juste avant) -- couvre
    // toute la fenetre, par-dessus le jeu.
    int winW, winH;
    SDL_GetWindowSize( window, &winW, &winH );

    SDL_SetRenderDrawColor( renderer, 10, 9, 8, 235 );
    SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_BLEND );
    SDL_Rect panel{ 0, kMenuBarHeight, winW, winH - kMenuBarHeight };
    SDL_RenderFillRect( renderer, &panel );
    SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_NONE );

    const RGBColor title{ 0xff, 0x48, 0x00 };
    const RGBColor text{ 0xd8, 0xd0, 0xc0 };
    const RGBColor dim{ 0x90, 0x88, 0x78 };
    int y = kMenuBarHeight + 4;
    constexpr int lh = 9; // hauteur de ligne, serree pour tenir a x2 (fenetre la plus petite)
    int x = 4;

    drawMenuText( x, y, "COMMANDES (touche PC = bouton AKA)", title ); y += lh + 2;
    drawMenuText( x, y, "Haut/Bas: avancer/reculer", text ); y += lh;
    drawMenuText( x, y, "Gauche/Droite: deplacement lateral", text ); y += lh;
    drawMenuText( x, y, "Q / E = L1/R1: tourner le regard", text ); y += lh;
    drawMenuText( x, y, "Z = A: valider/attaquer/utiliser", text ); y += lh;
    drawMenuText( x, y, "X = B: retour/refuser/jeter", text ); y += lh;
    drawMenuText( x, y, "C: inventaire   V: mini-carte", text ); y += lh + 4;

    drawMenuText( x, y, "AUTRES VERSIONS", title ); y += lh + 2;
    drawMenuText( x, y, "Console : Gamebuino AKA (ESP32-S3)", text ); y += lh;
    drawMenuText( x, y, "Cette build : PC de test (Windows/Linux)", text ); y += lh + 4;

    drawMenuText( x, y, "CREDITS", title ); y += lh + 2;
    drawMenuText( x, y, "Code/Art/Design orig. : Cyril Guichard", text ); y += lh;
    drawMenuText( x, y, "Concept original : Press Play On Tape", text ); y += lh;
    drawMenuText( x, y, "Musique (temp.) : \"Pyramid Level\",", text ); y += lh;
    drawMenuText( x, y, "  Visager, CC BY 4.0", dim ); y += lh;
    drawMenuText( x, y, "Police : \"Simple 5x8\", Atom596, FFC", text ); y += lh;
    drawMenuText( x, y, "Portage AKA/PC : Jicehel", text ); y += lh + 4;

    drawMenuText( x, y, "GITHUB", title ); y += lh + 2;
    drawMenuText( x, y, "github.com/Jicehel-Aka/", text ); y += lh;
    drawMenuText( x, y, "  dark_and_under_color", text );
}

void SdlRenderer::setZoom( int zoom ) {
    if ( zoom == currentZoom ) return;
    currentZoom = zoom;
    SDL_SetWindowSize( window, 150 * zoom, 81 * zoom + kMenuBarHeight );
}
