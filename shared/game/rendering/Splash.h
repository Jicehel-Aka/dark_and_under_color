// Splash.h — Porté depuis Splash.pde. Machine à états :
// CREDITS --A--> TITLE --A--> INTRO --A--> DONE (ferme le splash)
//                  ^---B (retour credits)---|
// Corrigé après retour de Jicehel : l'original a bien DEUX boutons sur
// l'écran-titre (rejouer les crédits, ou lancer l'intro), pas une
// simple progression linéaire comme la première version de ce fichier
// le faisait -- onActionPressed() unique remplacé par onButtonA/B.
// Le texte d'intro est celui de l'original (fichier fourni par
// Jicehel, projet dont il a l'autorisation de l'auteur).
#pragma once

#include "IRenderer.h"
#include "ITranslator.h"
#include "AssetIds.h"

enum class SplashState { Credits, Title, Intro, Done };

class Splash {
  public:
    SplashState getState() const { return state; }

    // À appeler sur pression fraîche du bouton d'action principal
    // (équivalent mousePressed+timer() de l'original) : Credits->Title,
    // Title->Intro (bouton PLAY), Intro->Done.
    void onButtonA();

    // Bouton retour : uniquement actif sur l'écran-titre, revient aux
    // crédits (bouton CREDITS de l'original). Sans effet ailleurs.
    void onButtonB();

    void render( IRenderer& renderer, ITranslator& translator );

  private:
    SplashState state = SplashState::Credits;
};
