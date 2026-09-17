// Assets.h — Contrat commun : chaque plateforme fournit sa propre
// résolution d'ImageId vers sa ressource réelle. Voir Assets.cpp dans
// chaque dossier platform_*/ pour l'implémentation concrète.
#pragma once

#include "IRenderer.h"

// SDL : nom de fichier PNG (dans data/) correspondant à cet ImageId, ou
// nullptr si inconnu.
const char* assetFileName( ImageId id );
