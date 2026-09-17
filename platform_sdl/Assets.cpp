#include "Assets.h"
#include "AssetIds.h"

const char* assetFileName( ImageId id ) {
    switch ( static_cast<ImageAsset>( id ) ) {
        case ImageAsset::ObjPotionIcon:   return "OBJ_potionHP.png";
        case ImageAsset::LootPotionFull:  return "LOOT_potionHP1.png";
        case ImageAsset::LootPotionMed:   return "LOOT_potionHP2.png";
        case ImageAsset::LootPotionSmall: return "LOOT_potionHP3.png";
        case ImageAsset::ObjKeyIcon:      return "OBJ_key.png";
        case ImageAsset::LootKeyFull:     return "LOOT_Key1.png";
        case ImageAsset::LootKeyMed:      return "LOOT_key2.png";
        case ImageAsset::LootKeySmall:    return "LOOT_key3.png";
        case ImageAsset::ObjMagicIcon:    return "OBJ_magic.png";
        case ImageAsset::LootMagicFull:   return "LOOT_magic1.png";
        case ImageAsset::LootMagicMed:    return "LOOT_magic2.png";
        case ImageAsset::LootMagicSmall:  return "LOOT_magic3.png";
        default: return nullptr;
    }
}
