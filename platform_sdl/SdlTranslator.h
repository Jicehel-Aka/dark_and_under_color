// SdlTranslator.h/.cpp — Charge un JSON plat (même format que les
// lang/*.json d'AKA : {"CLE": "texte"}, pas de JSON imbriqué) pour la
// build PC de test. Analyseur minimal maison plutôt qu'une dépendance
// JSON complète -- même principe que le chargeur interne
// d'aka_runtime.cpp (load_language_file_append), juste réécrit ici côté
// SDL puisqu'on ne peut pas appeler directement le code AKA.
//
// Charge desormais depuis les donnees EMBARQUEES dans l'executable
// (platform_sdl/embedded/embedded_lang.cpp, voir
// tools/embed_pc_assets.py) plutot que depuis des fichiers sur disque --
// demande par Jicehel.
#pragma once

#include "ITranslator.h"
#include <string>
#include <unordered_map>

class SdlTranslator : public ITranslator {
  public:
    // commonLangName/gameLangName : noms de fichiers EMBARQUES (ex.
    // "aka_common_fr.json", "fr.json"), pas des chemins disque. Charge
    // d'abord le fichier commun (s'il existe) puis le fichier du jeu,
    // comme le fait aka_runtime.
    SdlTranslator( const std::string& commonLangName, const std::string& gameLangName );

    const char* translate( const char* key ) override;

  private:
    void loadEmbedded( const std::string& name );
    std::unordered_map<std::string, std::string> entries;
};
