// SdlTranslator.h/.cpp — Charge un fichier JSON plat (même format que
// les lang/*.json d'AKA : {"CLE": "texte"}, pas de JSON imbriqué) pour
// la build PC de test. Analyseur minimal maison plutôt qu'une
// dépendance JSON complète -- même principe que le chargeur interne
// d'aka_runtime.cpp (load_language_file_append), juste réécrit ici côté
// SDL puisqu'on ne peut pas appeler directement le code AKA.
#pragma once

#include "ITranslator.h"
#include <string>
#include <unordered_map>

class SdlTranslator : public ITranslator {
  public:
    // langFile : chemin vers un fichier lang/<code>.json (ex.
    // "lang/fr.json"). Charge d'abord le fichier commun (s'il existe)
    // puis le fichier du jeu, comme le fait aka_runtime.
    SdlTranslator( const std::string& commonLangFile, const std::string& gameLangFile );

    const char* translate( const char* key ) override;

  private:
    void loadFile( const std::string& path );
    std::unordered_map<std::string, std::string> entries;
};
