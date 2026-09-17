// ITranslator.h — Le jeu ne connaît que cette interface pour tout texte
// affiché à l'écran. Côté AKA, implémentée par-dessus
// AkaRuntime::translate() (déjà chargé avec les fichiers communs
// /sdcard/AKA/lang/<code>.json ET spécifiques au jeu
// /sdcard/DarkUnderColor/lang/<code>.json -- même mécanisme que Dark &
// Under 1). Côté SDL, un chargeur JSON minimal pour tester sans le
// système AKA.
#pragma once

class ITranslator {
  public:
    virtual ~ITranslator() = default;

    // Renvoie le texte associé à "key", ou "key" lui-même si absent
    // (même comportement de repli que AkaRuntime::translate() --
    // jamais de chaîne vide/nulle, toujours quelque chose d'affichable
    // même si la traduction manque).
    virtual const char* translate( const char* key ) = 0;
};
