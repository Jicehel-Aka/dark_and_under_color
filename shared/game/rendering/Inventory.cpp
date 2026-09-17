#include "Inventory.h"
#include "Player.h"
#include <cstdio>

void Inventory::addItem( ObjectType type, const std::string& name ) {
    entries.push_back( { type, name } );
}

bool Inventory::takeSelected( InventoryEntry& out ) {
    if ( entries.empty() || selectedIndex < 0 || selectedIndex >= (int)entries.size() ) return false;
    out = entries[selectedIndex];
    entries.erase( entries.begin() + selectedIndex );
    if ( selectedIndex >= (int)entries.size() ) selectedIndex = (int)entries.size() - 1;
    return true;
}

void Inventory::selectNext() {
    if ( entries.empty() ) return;
    selectedIndex = ( selectedIndex + 1 ) % (int)entries.size();
}
void Inventory::selectPrev() {
    if ( entries.empty() ) return;
    selectedIndex = ( selectedIndex - 1 + (int)entries.size() ) % (int)entries.size();
}
void Inventory::switchTab() {
    activeTab = ( activeTab == InventoryTab::Items ) ? InventoryTab::Stats : InventoryTab::Items;
}

ImageAsset Inventory::iconFor( ObjectType type ) const {
    switch ( type ) {
        case ObjectType::Potion: return ImageAsset::ObjPotionIcon;
        case ObjectType::Key:    return ImageAsset::ObjKeyIcon;
        case ObjectType::Magic:  return ImageAsset::ObjMagicIcon;
    }
    return ImageAsset::ObjPotionIcon;
}

void Inventory::render( IRenderer& renderer, const Player& player, ITranslator& translator ) {
    renderer.drawImageScaled( 0, 0, 150 * 2, 81 * 2, (ImageId)ImageAsset::StatsParcheminLarge ); // meme PNG que Stats dans l'original (UI_parcheminLarge.png)

    if ( entries.empty() ) {
        renderer.drawText( 40 * 2, 30 * 2, translator.translate( "INV_EMPTY" ), RGBColor{ 0x19, 0x18, 0x14 } );
        return;
    }

    // Grille d'icones -- meme disposition que l'original (4 par ligne,
    // decalage de 17px). Selection marquee par l'image de surbrillance
    // d'origine (BUT_inventoryOver.png), pas un texte de secours.
    constexpr int kStartX = 16, kStartY = 16, kStep = 17;
    for ( size_t i = 0; i < entries.size(); ++i ) {
        int col = (int)( i % 4 );
        int row = (int)( i / 4 );
        int16_t x = (int16_t)( ( kStartX + kStep * col ) * 2 );
        int16_t y = (int16_t)( ( kStartY + kStep * row ) * 2 );
        renderer.drawImageScaled( x, y, 16 * 2, 16 * 2, (ImageId)iconFor( entries[i].type ) );
        if ( (int)i == selectedIndex ) {
            renderer.drawImageScaled( x, y, 16 * 2, 16 * 2, (ImageId)ImageAsset::InventorySelectHighlight );
        }
    }

    renderer.drawText( 16 * 2, 60 * 2, entries[selectedIndex].name.c_str(), RGBColor{ 0x24, 0x22, 0x1d } );
    renderer.drawText( 16 * 2, 68 * 2, translator.translate( "INV_USE_DISCARD" ), RGBColor{ 0x19, 0x18, 0x14 } );

    (void)player;
}
