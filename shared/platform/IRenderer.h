// IRenderer.h — Interface de rendu, commune aux deux cibles (AKA et
// futur build PC/SDL). Le code de jeu (game/) ne connaît QUE cette
// interface, jamais gb_graphics ni SDL directement -- c'est ce qui
// permet de compiler exactement le même code de jeu des deux côtés,
// avec juste l'implémentation concrète qui change.
//
// Les identifiants d'image (ImageId) sont des entiers opaques : côté
// AKA, ils indexent un tableau de buffers RGB565 en flash ; côté SDL,
// ils indexent un tableau de textures chargées depuis les PNG
// d'origine (chargement à chaud, bien plus simple pour itérer -- pas
// besoin de reconvertir les assets à chaque modification en cours de
// dev PC). Le jeu ne manipule jamais l'image elle-même, seulement son
// identifiant -- voir game/Assets.h pour la table de correspondance.
#pragma once

#include <cstdint>

using ImageId = uint16_t;
constexpr ImageId kInvalidImageId = 0xFFFF;

struct RGBColor {
    uint8_t r, g, b;
};

// Deux polices disponibles pour le texte du jeu -- Wide (8x8, celle de
// gb_graphics par defaut) et Narrow (5x8 "Simple 5x8", plus compacte).
// Retour de Jicehel apres test reel : le 8x8 convient mieux a l'ecran
// de victoire (peu de texte, le 5x8 y "fait vide"), le 5x8 reste
// necessaire pour le dialogue de butin (plus de texte a faire tenir).
// Wide par defaut pour ne pas casser les appels existants qui ne
// precisent rien.
enum class FontSize { Wide, Narrow };

class IRenderer {
  public:
    virtual ~IRenderer() = default;

    // Remplit un rectangle avec la couleur donnée (murs de la mini-carte
    // debug, fonds, etc.).
    virtual void fillRect( int16_t x, int16_t y, int16_t w, int16_t h, RGBColor color ) = 0;

    // Dessine une image à sa taille native.
    virtual void drawImage( int16_t x, int16_t y, ImageId image ) = 0;

    // Dessine une image redimensionnée (dst_w x dst_h) -- utilisé pour
    // le zoom AKA (x2) ; côté SDL, dst == taille native la plupart du
    // temps, sauf si on veut aussi zoomer la fenêtre de test.
    virtual void drawImageScaled( int16_t x, int16_t y, int16_t dst_w, int16_t dst_h, ImageId image ) = 0;

    // Dessine une REGION d'une feuille de sprites (atlas), redimensionnée.
    // Nécessaire pour la boussole (4 cadrans dans une seule image) et
    // les feuilles d'animation d'ennemis (3 frames) -- gb_graphics
    // l'expose nativement (voir gb_graphics_image.cpp), manquait juste
    // ici côté interface commune.
    virtual void drawImageRegionScaled( int16_t x, int16_t y, int16_t dst_w, int16_t dst_h,
                                         ImageId atlas, int16_t src_x, int16_t src_y,
                                         int16_t src_w, int16_t src_h ) = 0;

    // Texte simple (nom de niveau, stats, dialogues). size choisit la
    // police (voir FontSize) -- Wide par defaut si non precise.
    virtual void drawText( int16_t x, int16_t y, const char* text, RGBColor color, FontSize size = FontSize::Wide ) = 0;

    // Dimensions natives d'une image (nécessaire pour calculer où center
    // un sprite, par exemple) -- évite que le jeu ait à connaître le
    // détail des assets, juste leur taille.
    virtual void getImageSize( ImageId image, int16_t& outW, int16_t& outH ) const = 0;

    // Termine la frame et l'envoie à l'écran (gfx.update() côté AKA,
    // SDL_RenderPresent côté SDL).
    virtual void present() = 0;
};
