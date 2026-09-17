// AkaRenderer.h/.cpp — Implémentation AKA de IRenderer, par-dessus
// gb_graphics (même composant, avec les mêmes drawImage/drawImageScaled
// ajoutés lors du portage de Dark & Under 1 -- rien de nouveau à
// enrichir côté bibliothèque cette fois).
//
// Différence avec SdlRenderer : ici, un ImageId indexe un tableau de
// buffers RGB565 CONSTANTS EN FLASH (voir Assets.cpp), pas un fichier
// PNG chargé à chaud -- il faut convertir les PNG d'origine au
// préalable (outil à écrire, prochaine étape après ce squelette).
#pragma once

#include "IRenderer.h"
#include "gb_graphics.h"

struct AkaAsset {
    const uint16_t* pixels;
    uint16_t width;
    uint16_t height;
};

class AkaRenderer : public IRenderer {
  public:
    explicit AkaRenderer( gb_graphics& gfx ) : gfx( gfx ) {}

    void fillRect( int16_t x, int16_t y, int16_t w, int16_t h, RGBColor color ) override;
    void drawImage( int16_t x, int16_t y, ImageId image ) override;
    void drawImageScaled( int16_t x, int16_t y, int16_t dst_w, int16_t dst_h, ImageId image ) override;
    void drawImageRegionScaled( int16_t x, int16_t y, int16_t dst_w, int16_t dst_h,
                                 ImageId atlas, int16_t src_x, int16_t src_y,
                                 int16_t src_w, int16_t src_h ) override;
    void drawText( int16_t x, int16_t y, const char* text, RGBColor color, FontSize size = FontSize::Wide ) override;
    void getImageSize( ImageId image, int16_t& outW, int16_t& outH ) const override;
    void present() override;

  private:
    gb_graphics& gfx;
};

// Resolution ImageId -> buffer RGB565 embarque -- voir Assets.cpp.
const AkaAsset* getAkaAsset( ImageId id );
