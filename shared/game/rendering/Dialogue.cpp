#include "Dialogue.h"
#include <cstdio>
#include <cstring>

namespace {
// TAILLE DU PARCHEMIN : REVENUE A L'ORIGINAL (300x162, origine (0,0)) --
// trois passes d'agrandissement successives (330x178, puis 350x190,
// puis 370x200) ont ete annulees. Cause reelle du "parchemin trop
// petit" signale a chaque test : ce n'etait JAMAIS une question de
// taille. La vraie boucle draw() de l'original (darkUnderCOLOR.pde)
// dessine Vision+UI EN PERMANENCE, quel que soit l'ecran actif, et
// superpose Dialogue PAR-DESSUS (voir GameApp::render()) -- le
// parchemin (UI_parchemin.png, 150x81, avec ses marges transparentes
// sur les cotes) a toujours ete cense laisser voir le panneau de droite
// du dessous. Cette couche manquante donnait l'impression d'un
// parchemin trop petit ; corrige au bon endroit (GameApp), pas ici.
constexpr int16_t kParchmentCenterX = 105; // REVENU a cette valeur -- la correction precedente (65) partait d'une fausse generalisation (meme cause que la bande du bas) ; Jicehel confirme que cette zone etait deja correcte a part "Continuez" specifiquement, pas un decalage global a appliquer ici
constexpr int16_t kParchmentDrawW = 320; // elargi vers la droite (etait 300) -- origine (0,0) inchangee
constexpr int16_t kParchmentDrawH = 182; // elargi vers le bas (etait 162)

constexpr int16_t kNarrowCharWidthPx = 6; // police 5x8 "Simple 5x8" (licence FFC 1001fonts)
constexpr int16_t kWideCharWidthPx = 8;   // police 8x8 par defaut de gb_graphics

int16_t centeredX( const char* text, FontSize size ) {
    int16_t charW = ( size == FontSize::Wide ) ? kWideCharWidthPx : kNarrowCharWidthPx;
    int16_t textWidth = (int16_t)( std::strlen( text ) * charW );
    int16_t x = (int16_t)( kParchmentCenterX - textWidth / 2 );
    return x < 0 ? 0 : x;
}
}

void Dialogue::showLoot( ObjectType type, const std::string& name ) {
    kind = DialogueKind::Loot;
    lootType = type;
    itemName = name;
}
void Dialogue::showWinFight( const std::string& enemyName, int xp ) {
    kind = DialogueKind::WinFight;
    itemName = enemyName;
    xpAwarded = xp;
}

ImageAsset Dialogue::lootIconFor( ObjectType type ) const {
    switch ( type ) {
        case ObjectType::Potion: return ImageAsset::LootPotionFull;
        case ObjectType::Key:    return ImageAsset::LootKeyFull;
        case ObjectType::Magic:  return ImageAsset::LootMagicFull;
    }
    return ImageAsset::LootPotionFull;
}

void Dialogue::render( IRenderer& renderer, ITranslator& translator ) {
    // Taille elargie vers la droite/le bas, origine (0,0) inchangee --
    // demande par Jicehel apres avoir confirme que l'ecran ne pose plus
    // de probleme depuis la correction de la couche Vision+UI en
    // dessous (voir GameApp) : la marge supplementaire donne de la
    // place pour le cercle XP agrandi.
    renderer.drawImageScaled( 0, 0, kParchmentDrawW, kParchmentDrawH, (ImageId)ImageAsset::MinimapParchemin );

    if ( kind == DialogueKind::Loot ) {
        // Butin : police Narrow (5x8) -- plus de texte a faire tenir
        // ("A:GARDE B:NON").
        renderer.drawImageScaled( kParchmentCenterX - 32, 10 * 2, 32 * 2, 32 * 2, (ImageId)lootIconFor( lootType ) );

        const char* foundText = translator.translate( "DLG_YOU_FOUND" );
        renderer.drawText( centeredX( foundText, FontSize::Narrow ), 44 * 2, foundText, RGBColor{ 0x24, 0x22, 0x1d }, FontSize::Narrow );
        renderer.drawText( centeredX( itemName.c_str(), FontSize::Narrow ), 50 * 2, itemName.c_str(), RGBColor{ 0xff, 0x48, 0x00 }, FontSize::Narrow );
        const char* takeLeaveText = translator.translate( "DLG_TAKE_LEAVE" );
        renderer.drawText( centeredX( takeLeaveText, FontSize::Narrow ), 58 * 2, takeLeaveText, RGBColor{ 0x19, 0x18, 0x14 }, FontSize::Narrow );

    } else if ( kind == DialogueKind::WinFight ) {
        // Victoire : police Wide (8x8) -- moins de texte, le 5x8
        // laissait trop de vide (retour de Jicehel apres test reel).
        // Cercle XP legerement agrandi (32x32 -> 36x36) et chiffre
        // CENTRE DESSUS (pas "+10 XP" en dessous) -- corrige pour
        // matcher l'original exactement : Dialogue.pde ne dessine que
        // le nombre brut par-dessus l'icone (text(10,49*multi,25*multi)),
        // jamais de "+" ni de suffixe "XP" separes (le mot "XP" est deja
        // dans le dessin de l'icone elle-meme).
        constexpr int16_t kXpIconSize = 36 * 2;
        int16_t xpIconX = kParchmentCenterX - kXpIconSize / 2;
        renderer.drawImageScaled( xpIconX, 8 * 2, kXpIconSize, kXpIconSize, (ImageId)ImageAsset::LootXP );

        const char* killedText = translator.translate( "DLG_YOU_KILLED" );
        renderer.drawText( centeredX( killedText, FontSize::Wide ), 45 * 2, killedText, RGBColor{ 0x24, 0x22, 0x1d }, FontSize::Wide );
        renderer.drawText( centeredX( itemName.c_str(), FontSize::Wide ), 51 * 2, itemName.c_str(), RGBColor{ 0xff, 0x48, 0x00 }, FontSize::Wide );

        char xpLine[16];
        std::snprintf( xpLine, sizeof( xpLine ), "%d", xpAwarded ); // juste le nombre, comme l'original -- pas de "+", pas de "XP" (deja dans l'icone)
        renderer.drawText( centeredX( xpLine, FontSize::Wide ), 22 * 2, xpLine, RGBColor{ 0xb4, 0xff, 0x00 }, FontSize::Wide );

        const char* continueText = translator.translate( "DLG_CONTINUE" );
        // Decalage cible de 20px vers la gauche (estimation "a la louche"
        // de Jicehel) -- le mecanisme de centrage lui-meme (centeredX,
        // meme police Wide que les autres textes de cet ecran) ne
        // presente pas d'incoherence de code identifiable ; ajustement
        // isole a cette chaine specifique plutot qu'une modification du
        // calcul general, qui semble correct pour le reste de cet ecran.
        // 2e ajustement (retour de Jicehel : maintenant trop a gauche,
        // "d'une lettre" sur AKA comme sur PC -- le -20px precedent
        // etait une estimation "a la louche", ajustee ici de +8px
        // (une largeur de caractere Wide) vers la droite.
        renderer.drawText( centeredX( continueText, FontSize::Wide ) - 12, 58 * 2, continueText, RGBColor{ 0x19, 0x18, 0x14 }, FontSize::Wide );
    }
}
