// PlatformTime.h — Équivalent de millis() (Processing/Arduino), une
// seule fonction que chaque plateforme implémente (AKA :
// esp_timer_get_time()/1000 ; SDL : SDL_GetTicks()). Utilisé par les
// portes temporelles type timer()/animTimer() de l'original (voir
// Enemy.h).
//
// BUG TROUVE ET CORRIGE (build Windows/MSVC echouait sur
// SdlTranslator.cpp avec "'clock_t' is not a member of global
// namespace") : ce fichier s'appelait "Time.h" -- sur Linux/macOS
// (systemes de fichiers sensibles a la casse), ca ne pose aucun
// probleme, mais Windows/NTFS est insensible a la casse : "Time.h" et
// "time.h" sont LE MEME FICHIER pour lui. Comme shared/platform/ est
// dans le chemin d'inclusion, ce fichier ecrasait le vrai <time.h> du
// CRT MSVC des qu'un header standard (<ctime> ici, via SdlTranslator.cpp)
// tentait de l'inclure -- renomme pour eviter toute collision.
#pragma once

#include <cstdint>

uint32_t platformMillis();
