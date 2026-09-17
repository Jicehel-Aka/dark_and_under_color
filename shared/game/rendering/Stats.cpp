#include "Stats.h"
#include "../entities/Player.h"
#include <cstdio>

void Stats::render( IRenderer& renderer, const Player& player, ITranslator& translator ) {
    renderer.drawImageScaled( 0, 0, 150 * 2, 81 * 2, (ImageId)ImageAsset::StatsParcheminLarge );
    renderer.drawImageScaled( 14 * 2, 8 * 2, 24 * 2, 33 * 2, (ImageId)ImageAsset::StatsPortrait );

    renderer.drawText( 40 * 2, 14 * 2, translator.translate( "STATS_HERO_NAME" ), RGBColor{ 0x19, 0x18, 0x14 } );

    char line[32];
    std::snprintf( line, sizeof( line ), "HP:%d/%d", (int)player.getCurrentHP(), (int)player.getMaxHP() );
    renderer.drawText( 40 * 2, 20 * 2, line, RGBColor{ 0xff, 0x48, 0x00 } );

    std::snprintf( line, sizeof( line ), "XP:%d/%d", (int)player.getCurrentXP(), (int)player.getMaxXP() );
    renderer.drawText( 40 * 2, 26 * 2, line, RGBColor{ 0x85, 0xbc, 0x00 } );

    renderer.drawImageScaled( 40 * 2, 27 * 2, 5 * 2, 5 * 2, (ImageId)ImageAsset::StatsSymbolAP );
    renderer.drawImageScaled( 40 * 2, 33 * 2, 5 * 2, 5 * 2, (ImageId)ImageAsset::StatsSymbolDF );

    std::snprintf( line, sizeof( line ), "AP:%d", (int)player.getAP() );
    renderer.drawText( 46 * 2, 32 * 2, line, RGBColor{ 0x19, 0x18, 0x14 } );
    std::snprintf( line, sizeof( line ), "DF:%d", (int)player.getDF() );
    renderer.drawText( 46 * 2, 38 * 2, line, RGBColor{ 0x19, 0x18, 0x14 } );

    renderer.drawImageScaled( 10 * 2, 46 * 2, 80 * 2, 23 * 2, (ImageId)ImageAsset::StatsBackButton );
}
