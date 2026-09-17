// Time.h — Équivalent de millis() (Processing/Arduino), une seule
// fonction que chaque plateforme implémente (AKA : esp_timer_get_time()/1000 ;
// SDL : SDL_GetTicks()). Utilisé par les portes temporelles type
// timer()/animTimer() de l'original (voir Enemy.h).
#pragma once

#include <cstdint>

uint32_t platformMillis();
