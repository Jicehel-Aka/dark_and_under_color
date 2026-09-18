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
    // Hauteur FIXE de la barre de menu (retour de Jicehel : menu de
    // zoom en haut de fenetre) -- en pixels REELS, ne change pas avec
    // le zoom (sinon elle deviendrait illisible a x2 ou enorme a x4).
    static constexpr int kMenuBarHeight = 20;

    // zoom : x2/x3/x4 (voir setZoom) -- assetDir : dossier contenant
    // les PNG d'origine (le "data/" du projet Processing, réutilisé
    // tel quel -- voir Assets.h pour la table ImageId -> nom de fichier).
    SdlRenderer( int zoom, const std::string& assetDir );
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

    // A appeler EN DEBUT de chaque frame, avant le rendu du jeu lui
    // meme (Vision/UI/...) : positionne la zone de dessin (viewport +
    // echelle) sous la barre de menu -- le jeu continue de dessiner
    // exactement comme avant (coordonnees "300x162", zoom x2 deja
    // integre dans le code de jeu), c'est ici que le zoom
    // SUPPLEMENTAIRE (x3/x4) et le decalage pour la barre sont
    // appliques, sans toucher a un seul appel de dessin du jeu.
    void beginGameArea();

    // Dessine "ZOOM :  x2  x3  x4  [?]" en haut de la fenetre, en
    // coordonnees REELLES (pas affectees par le zoom du jeu -- taille
    // de texte constante quel que soit le zoom choisi). A appeler APRES
    // le rendu du jeu, avant present(). Renvoie le zoom sur lequel
    // l'utilisateur vient de cliquer (0 si aucun clic ce tour-ci) --
    // c'est a l'appelant (main.cpp) de decider d'appliquer setZoom()
    // avec cette valeur. Le bouton "?" bascule lui-meme
    // isHelpPanelOpen() en interne (pas besoin que l'appelant s'en
    // occupe, contrairement au zoom qui modifie la taille de fenetre).
    int renderMenuBar( int mouseX, int mouseY, bool mouseClicked );

    // Panneau d'aide demande par Jicehel (commandes PC/touche AKA
    // correspondante, autres versions, credits, lien GitHub) -- a
    // appeler juste apres renderMenuBar() si isHelpPanelOpen() est vrai.
    // Couvre toute la fenetre en coordonnees reelles, par-dessus le jeu.
    void renderHelpPanel();
    bool isHelpPanelOpen() const { return helpPanelOpen; }

    // Change le zoom EN COURS D'EXECUTION -- redimensionne la fenetre
    // reelle (SDL_SetWindowSize), la zone de jeu s'adapte automatiquement
    // au prochain beginGameArea().
    void setZoom( int zoom );
    int getZoom() const { return currentZoom; }

  private:
    SDL_Texture* getTexture( ImageId image );
    void drawMenuText( int x, int y, const char* text, RGBColor color ); // texte en coordonnees reelles, non affecte par le zoom du jeu

    std::string assetDir;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::unordered_map<ImageId, SDL_Texture*> textureCache;
    int currentZoom = 2;
    bool helpPanelOpen = false;
};
