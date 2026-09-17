// AssetIds.h — Les identifiants d'image que le jeu manipule. Partagés
// entre toutes les plateformes : le jeu ne connaît que ces noms, jamais
// un chemin de fichier PNG ni un buffer RGB565 -- la résolution
// ImageId -> ressource réelle est de la responsabilité de chaque
// plateforme (voir platform_sdl/Assets.cpp pour les noms de fichiers,
// futur platform_aka/Assets.cpp pour les buffers embarqués).
//
// Liste initiale limitée aux objets ramassables (LOOT_*/OBJ_*), le
// strict nécessaire pour Object.h porté jusqu'ici -- complétée au fur
// et à mesure du portage des autres systèmes (murs, ennemis, UI...).
#pragma once

#include <cstdint>

enum class ImageAsset : uint16_t {
    // Objets ramassables -- inventaire, taille pleine, moyenne, petite
    // (3 paliers de distance, comme sur Dark & Under 1).
    ObjPotionIcon, LootPotionFull, LootPotionMed, LootPotionSmall,
    ObjKeyIcon,    LootKeyFull,    LootKeyMed,    LootKeySmall,
    ObjMagicIcon,  LootMagicFull,  LootMagicMed,  LootMagicSmall,
    LootXP,

    // Fond du couloir (2 variantes, alternées à chaque pas -- oddStep
    // dans l'original -- pour un effet de balancement de la caméra).
    VisionBack, VisionBack2,

    // Murs du couloir en perspective. "Farest" (le coin extrême, 3 cases
    // devant) n'a qu'une seule texture quel que soit le type de sol.
    // Les autres existent chacun en 2 variantes (Wall=1 -> normal,
    // Special=2 -> variante alternative -- CONFIRME en lisant Vision.pde :
    // Special n'est PAS une porte/sortie comme on le supposait au
    // moment de porter Level, juste une texture de mur alternative,
    // purement decorative -- la case reste bloquante pour le
    // deplacement dans les deux cas, voir TileType.h mis a jour).
    FarWallFront,     // pas de variante _2 (toujours la meme texture)
    FarestWallLeft, FarestWallRight, // jamais de variante non plus
    FarWallLeft, FarWallLeft2, FarWallRight, FarWallRight2,
    MidWallFront, MidWallFront2,
    MidWallLeft, MidWallLeft2, MidWallRight, MidWallRight2,
    CloseWallFront, CloseWallFront2,
    CloseWallLeft, CloseWallLeft2, CloseWallRight, CloseWallRight2,

    // Icônes moyenne/petite d'objets et d'ennemis vus au loin dans le
    // couloir (distinctes des icônes d'inventaire déjà listées plus haut).
    EnemyRatIconMed, EnemyRatIconSmall,
    EnemySkeletonIconMed, EnemySkeletonIconSmall,

    // Mini-carte (Minimap.pde).
    MinimapParchemin, WallMapMarker,
    PlayerMapMarker1, PlayerMapMarker2, PlayerMapMarker3, PlayerMapMarker4,
    ObjectMapMarker, EnemyMapMarker,

    // Écran de statistiques du héros (Stats.pde).
    StatsParcheminLarge, StatsPortrait, StatsSymbolAP, StatsSymbolDF, StatsBackButton,

    // Écran splash/titre/intro (Splash.pde).
    SplashCredits, SplashTitleScreen, SplashIntro, SplashLargeButton, SplashLargeButtonOver,
    InventorySelectHighlight,

    // UI permanente pendant l'exploration/le combat (UI.pde).
    UIMain, UICompass, // UICompass : atlas 36x9, 4 cadrans de 9x9 (voir UI.cpp)
    EnemyScratch, EnemySplat, EnemyHPBar, UISwordHit,
    EnemyRatSpriteSheet, EnemySkeletonSpriteSheet, // atlas 192x64, 3 frames de 64x64

    // Icones de direction (D-pad) -- gardees comme repere visuel statique
    // (retour de Jicehel : visibles et blanches dans l'original) meme si
    // non cliquables ici (remplacees par de vrais boutons physiques).
    ArrowForward, ArrowBackward, ArrowLeft, ArrowRight, ArrowRotateLeft, ArrowRotateRight,

    Count, // sentinelle -- nombre total d'assets, ne pas utiliser comme ID
};
