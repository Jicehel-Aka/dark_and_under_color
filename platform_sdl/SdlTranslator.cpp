#include "SdlTranslator.h"
#include "embedded/embedded_assets.h"

SdlTranslator::SdlTranslator( const std::string& commonLangName, const std::string& gameLangName ) {
    loadEmbedded( commonLangName );
    loadEmbedded( gameLangName );
}

void SdlTranslator::loadEmbedded( const std::string& name ) {
    const EmbeddedAsset* asset = findEmbeddedLang( name.c_str() );
    if ( asset == nullptr ) return; // absent -- pas grave, translate() repliera sur la cle

    std::string content( reinterpret_cast<const char*>( asset->data ), asset->size );

    // Analyseur minimal : cherche des paires "CLE": "valeur" -- suffisant
    // pour le format plat utilise par tous les lang/*.json du projet
    // (memes fichiers que ceux lus par aka_runtime.cpp cote AKA).
    size_t pos = 0;
    while ( true ) {
        size_t keyStart = content.find( '"', pos );
        if ( keyStart == std::string::npos ) break;
        size_t keyEnd = content.find( '"', keyStart + 1 );
        if ( keyEnd == std::string::npos ) break;
        std::string key = content.substr( keyStart + 1, keyEnd - keyStart - 1 );

        size_t colon = content.find( ':', keyEnd );
        if ( colon == std::string::npos ) break;
        size_t valStart = content.find( '"', colon );
        if ( valStart == std::string::npos ) break;
        size_t valEnd = valStart + 1;
        std::string value;
        while ( valEnd < content.size() && content[valEnd] != '"' ) {
            if ( content[valEnd] == '\\' && valEnd + 1 < content.size() ) {
                ++valEnd;
                if ( content[valEnd] == 'n' ) value += '\n';
                else value += content[valEnd];
            } else {
                value += content[valEnd];
            }
            ++valEnd;
        }
        if ( valEnd >= content.size() ) break;

        entries[key] = value;
        pos = valEnd + 1;
    }
}

const char* SdlTranslator::translate( const char* key ) {
    auto it = entries.find( key );
    if ( it != entries.end() ) return it->second.c_str();
    return key; // repli identique a AkaRuntime::translate()
}
