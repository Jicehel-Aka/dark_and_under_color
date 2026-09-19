// embedded_assets.h — Structure commune + déclarations pour les assets
// PC embarqués directement dans l'exécutable (images/langues/musique),
// générés par tools/embed_pc_assets.py. Voir ce script pour le format
// exact des .cpp générés dans ce même dossier.
#pragma once

#include <cstring>

struct EmbeddedAsset {
    const char* name;
    const unsigned char* data;
    unsigned int size;
};

const EmbeddedAsset* findEmbeddedImage( const char* name );
const EmbeddedAsset* findEmbeddedLang( const char* name );
const EmbeddedAsset* findEmbeddedMusic( const char* name );
