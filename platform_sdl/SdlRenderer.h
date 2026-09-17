// SdlRenderer.h/.cpp — Implémentation SDL2 de IRenderer, pour la build
// PC de test. Différence volontaire avec la future implémentation AKA :
// ici, les images sont chargées directement depuis les PNG d'origine à
// l'exécution (SDL_image), pas depuis des buffers RGB565 précompilés --
// bien plus simple pour itérer en cours de dev (changer un PNG, relancer,
// c'est tout), la conversion en assets embarqués ne concerne que la
// build AKA finale. Les deux respectent la même interface, donc le code
// de jeu ne voit aucune différence.
#pragma once

#include "IRenderer.h"
#include "fonts/simple5x8_font.h"
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

class SdlRenderer : public IRenderer {
  public:
    // assetDir : dossier contenant les PNG d'origine (le "data/" du
    // projet Processing, réutilisé tel quel -- voir Assets.h pour la
    // table ImageId -> nom de fichier).
    SdlRenderer( int windowWidth, int windowHeight, const std::string& assetDir );
    ~SdlRenderer() override;

    void fillRect( int16_t x, int16_t y, int16_t w, int16_t h, RGBColor color ) override;
    void drawImage( int16_t x, int16_t y, ImageId image ) override;
    void drawImageScaled( int16_t x, int16_t y, int16_t dst_w, int16_t dst_h, ImageId image ) override;
    void drawImageRegionScaled( int16_t x, int16_t y, int16_t dst_w, int16_t dst_h,
                                 ImageId atlas, int16_t src_x, int16_t src_y,
                                 int16_t src_w, int16_t src_h ) override;
    void drawText( int16_t x, int16_t y, const char* text, RGBColor color, FontSize size = FontSize::Wide ) override;
    void getImageSize( ImageId image, int16_t& outW, int16_t& outH ) const override;
    void present() override;

    bool ok() const { return window != nullptr && renderer != nullptr; }

    // BUG TROUVE ET CORRIGE (musique OK mais ecran noir, signale par
    // Jicehel) : un echec de chargement d'assets ne produisait qu'un
    // message dans stderr, invisible sur un exe lance en double-clic
    // (pas de console attachee). Verifie qu'un asset connu se charge
    // bien AU DEMARRAGE et affiche une VRAIE fenetre d'erreur (visible
    // meme sans console) sinon, avec le chemin exact tente -- le joueur
    // voit directement pourquoi, plutot que de deviner face a un ecran
    // noir muet.
    bool verifyAssetsLoadable();

  private:
    SDL_Texture* getTexture( ImageId image );

    std::string assetDir;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::unordered_map<ImageId, SDL_Texture*> textureCache;
};
