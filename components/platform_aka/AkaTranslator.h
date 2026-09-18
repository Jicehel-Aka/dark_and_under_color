// AkaTranslator.h — Implémentation ITranslator pour AKA : simple
// enveloppe autour d'AkaRuntime::translate(), qui charge deja les deux
// fichiers de langue (commun /sdcard/AKA/lang/ + specifique au jeu
// /sdcard/DarkUnderColor/lang/, voir sdcard_files/) et fusionne les deux
// en une seule table de recherche. Voir SdlTranslator.h pour
// l'equivalent PC (charge les memes fichiers directement, sans passer
// par aka_runtime qui n'existe que sur la console).
#pragma once

#include "ITranslator.h"
#include "aka_runtime/aka_runtime.h"

class AkaTranslator : public ITranslator {
  public:
    const char* translate( const char* key ) override {
        return akaRuntime.translate( key );
    }
};
