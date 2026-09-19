// Assets.cpp — GENERE AUTOMATIQUEMENT par tools/convert_assets.py
// (meme config que components/platform_aka/GeneratedAssets.cpp --
// ne plus editer a la main, les deux fichiers derivaient l'un de
// l'autre auparavant, cause d'un ecran noir cote SDL).
#include "Assets.h"
#include "AssetIds.h"

const char* assetFileName( ImageId id ) {
    switch ( static_cast<ImageAsset>( id ) ) {
        case ImageAsset::ObjPotionIcon: return "OBJ_potionHP.png";
        case ImageAsset::LootPotionFull: return "LOOT_potionHP1.png";
        case ImageAsset::LootPotionMed: return "LOOT_potionHP2.png";
        case ImageAsset::LootPotionSmall: return "LOOT_potionHP3.png";
        case ImageAsset::ObjKeyIcon: return "OBJ_key.png";
        case ImageAsset::LootKeyFull: return "LOOT_Key1.png";
        case ImageAsset::LootKeyMed: return "LOOT_key2.png";
        case ImageAsset::LootKeySmall: return "LOOT_key3.png";
        case ImageAsset::ObjMagicIcon: return "OBJ_magic.png";
        case ImageAsset::LootMagicFull: return "LOOT_magic1.png";
        case ImageAsset::LootMagicMed: return "LOOT_magic2.png";
        case ImageAsset::LootMagicSmall: return "LOOT_magic3.png";
        case ImageAsset::VisionBack: return "dungeon_back1.png";
        case ImageAsset::VisionBack2: return "dungeon_back2.png";
        case ImageAsset::FarWallFront: return "dungeon_frontWall3.png";
        case ImageAsset::FarestWallLeft: return "dungeon_leftWall4.png";
        case ImageAsset::FarestWallRight: return "dungeon_rightWall4.png";
        case ImageAsset::FarWallLeft: return "dungeon_leftWall3.png";
        case ImageAsset::FarWallLeft2: return "dungeon_leftWall3_2.png";
        case ImageAsset::FarWallRight: return "dungeon_rightWall3.png";
        case ImageAsset::FarWallRight2: return "dungeon_rightWall3_2.png";
        case ImageAsset::MidWallFront: return "dungeon_frontWall2.png";
        case ImageAsset::MidWallFront2: return "dungeon_frontWall2_2.png";
        case ImageAsset::MidWallLeft: return "dungeon_leftWall2.png";
        case ImageAsset::MidWallLeft2: return "dungeon_leftWall2_2.png";
        case ImageAsset::MidWallRight: return "dungeon_rightWall2.png";
        case ImageAsset::MidWallRight2: return "dungeon_rightWall2_2.png";
        case ImageAsset::CloseWallFront: return "dungeon_frontWall1.png";
        case ImageAsset::CloseWallFront2: return "dungeon_frontWall1_2.png";
        case ImageAsset::CloseWallLeft: return "dungeon_leftWall1.png";
        case ImageAsset::CloseWallLeft2: return "dungeon_leftWall1_2.png";
        case ImageAsset::CloseWallRight: return "dungeon_rightWall1.png";
        case ImageAsset::CloseWallRight2: return "dungeon_rightWall1_2.png";
        case ImageAsset::EnemyRatIconMed: return "ENM_rat2.png";
        case ImageAsset::EnemyRatIconSmall: return "ENM_rat3.png";
        case ImageAsset::EnemySkeletonIconMed: return "ENM_skelly2.png";
        case ImageAsset::EnemySkeletonIconSmall: return "ENM_skelly3.png";
        case ImageAsset::MinimapParchemin: return "UI_parchemin.png";
        case ImageAsset::WallMapMarker: return "UI_wallMapMarker.png";
        case ImageAsset::PlayerMapMarker1: return "UI_playerMapMarker.png";
        case ImageAsset::PlayerMapMarker2: return "UI_playerMapMarker2.png";
        case ImageAsset::PlayerMapMarker3: return "UI_playerMapMarker3.png";
        case ImageAsset::PlayerMapMarker4: return "UI_playerMapMarker4.png";
        case ImageAsset::ObjectMapMarker: return "UI_objectMapMarker.png";
        case ImageAsset::EnemyMapMarker: return "UI_enemyMapMarker.png";
        case ImageAsset::StatsParcheminLarge: return "UI_parcheminLarge.png";
        case ImageAsset::StatsPortrait: return "UI_portraitFemale.png";
        case ImageAsset::StatsSymbolAP: return "UI_symbolAP.png";
        case ImageAsset::StatsSymbolDF: return "UI_symbolDF.png";
        case ImageAsset::StatsBackButton: return "BUT_quadBack.png";
        case ImageAsset::SplashCredits: return "garCoLogo.png";
        case ImageAsset::SplashTitleScreen: return "UI_titleScreen.png";
        case ImageAsset::SplashIntro: return "UI_intro.png";
        case ImageAsset::UIMain: return "UI_main.png";
        case ImageAsset::UICompass: return "UI_compass.png";
        case ImageAsset::EnemyScratch: return "ENM_scratch.png";
        case ImageAsset::EnemySplat: return "ENM_splat.png";
        case ImageAsset::EnemyHPBar: return "ENM_HPBar.png";
        case ImageAsset::UISwordHit: return "UI_swordHit.png";
        case ImageAsset::EnemyRatSpriteSheet: return "ENM_rat.png";
        case ImageAsset::EnemySkeletonSpriteSheet: return "ENM_skelly.png";
        case ImageAsset::LootXP: return "LOOT_XP.png";
        case ImageAsset::InventorySelectHighlight: return "BUT_inventoryOver.png";
        case ImageAsset::ArrowForward: return "BUT_forward.png";
        case ImageAsset::ArrowBackward: return "BUT_backwards.png";
        case ImageAsset::ArrowLeft: return "BUT_left.png";
        case ImageAsset::ArrowRight: return "BUT_right.png";
        case ImageAsset::ArrowRotateLeft: return "BUT_turnLeft.png";
        case ImageAsset::ArrowRotateRight: return "BUT_turnRight.png";
        case ImageAsset::IconSword: return "BUT_sword.png";
        case ImageAsset::IconMagic: return "BUT_magic.png";
        case ImageAsset::IconPotion: return "BUT_potion.png";
        case ImageAsset::IconShield: return "BUT_shield.png";
        case ImageAsset::SplashLargeButton: return "BUT_largeBut.png";
        default: return nullptr;
    }
}
