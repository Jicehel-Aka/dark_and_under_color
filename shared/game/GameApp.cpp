#include "GameApp.h"
#include "RelativeMovement.h"
#include <cstring>
#include <cstdio>

namespace {
GameObject makeObject( const ObjectSpawn& spawn ) {
    ObjectType type;
    switch ( spawn.type ) {
        case 'P': type = ObjectType::Potion; break;
        case 'K': type = ObjectType::Key;    break;
        case 'M': type = ObjectType::Magic;  break;
        default:  type = ObjectType::Potion; break;
    }
    GameObject o( spawn.col, spawn.row, type, spawn.id );
    o.setActive( true );
    return o;
}
Enemy makeEnemy( const EnemySpawn& spawn ) {
    EnemyType type = ( spawn.type == 'S' ) ? EnemyType::Skeleton : EnemyType::Rat;
    return Enemy( spawn.col, spawn.row, type, spawn.id );
}
const char* objectTranslationKey( ObjectType type ) {
    switch ( type ) {
        case ObjectType::Potion: return "ITEM_POTION";
        case ObjectType::Key:    return "ITEM_KEY";
        case ObjectType::Magic:  return "ITEM_SCROLL";
    }
    return "";
}
}

GameApp::GameApp( const LevelData& levelData, ITranslator& translatorRef )
    : level( levelData )
    , player( levelData.startRow, levelData.startCol,
              [this]( int row, int col ) { return level.isTileWalkable( row, col ); },
              [this]() { vision.setOddStep( !vision.getOddStep() ); } )
    , translator( translatorRef )
{
    for ( int i = 0; i < levelData.objectCount; ++i ) objects.push_back( makeObject( levelData.objects[i] ) );
    for ( int i = 0; i < levelData.enemyCount; ++i ) enemies.push_back( makeEnemy( levelData.enemies[i] ) );
}

void GameApp::checkTriggers() {
    for ( size_t i = 0; i < enemies.size(); ++i ) {
        if ( enemies[i].checkCollision( enemies[i].getCol(), enemies[i].getRow(), player.getCol(), player.getRow() ) ) {
            mode = ScreenMode::Combat;
            activeEnemyIndex = (int)i;
            lastSeenFightState = FightState::EnemyAttacking;
            char buf[48];
            std::snprintf( buf, sizeof( buf ), translator.translate( "CBT_ATTACKS" ), enemies[i].getDescription() );
            combatMessage = buf;
            return;
        }
    }
    for ( size_t i = 0; i < objects.size(); ++i ) {
        if ( !objects[i].isActive() ) continue;
        if ( objects[i].checkCollision( objects[i].getCol(), objects[i].getRow(), player.getCol(), player.getRow() ) ) {
            dialogue.showLoot( objects[i].getType(), translator.translate( objectTranslationKey( objects[i].getType() ) ) );
            pendingLootObjectIndex = (int)i;
            modeBeforeDialogue = ScreenMode::Exploring;
            mode = ScreenMode::DialogueView;
            return;
        }
    }
}

void GameApp::update( const IInput& input ) {
    switch ( mode ) {
        case ScreenMode::Splash:
            if ( input.justPressed( &InputState::actionA ) ) {
                splash.onButtonA();
                if ( splash.getState() == SplashState::Done ) mode = ScreenMode::Exploring;
            } else if ( input.justPressed( &InputState::actionB ) ) {
                splash.onButtonB();
            }
            break;

        case ScreenMode::Exploring:
            updateExploring( input );
            checkTriggers();
            break;

        case ScreenMode::Combat:
            updateCombat( input );
            break;

        case ScreenMode::MinimapView:
            if ( input.justPressed( &InputState::actionD ) || input.justPressed( &InputState::actionB ) )
                mode = ScreenMode::Exploring;
            break;

        case ScreenMode::Inventory:
            updateInventory( input );
            break;

        case ScreenMode::DialogueView:
            updateDialogue( input );
            break;
    }
}

void GameApp::updateExploring( const IInput& input ) {
    FacingDirection facing = vision.getDirection();

    if ( input.justPressed( &InputState::up ) )    player.movePlayer( relativeForward( facing ) );
    if ( input.justPressed( &InputState::down ) )  player.movePlayer( relativeBackward( facing ) );
    if ( input.justPressed( &InputState::left ) )  player.movePlayer( relativeStrafeLeft( facing ) );
    if ( input.justPressed( &InputState::right ) ) player.movePlayer( relativeStrafeRight( facing ) );

    if ( input.justPressed( &InputState::l1 ) ) vision.setDirection( rotateLeft( facing ) );
    if ( input.justPressed( &InputState::r1 ) ) vision.setDirection( rotateRight( facing ) );

    if ( input.justPressed( &InputState::actionC ) ) { modeBeforeInventory = ScreenMode::Exploring; mode = ScreenMode::Inventory; }
    if ( input.justPressed( &InputState::actionD ) ) mode = ScreenMode::MinimapView;
}

void GameApp::updateCombat( const IInput& input ) {
    if ( activeEnemyIndex < 0 || activeEnemyIndex >= (int)enemies.size() ) {
        mode = ScreenMode::Exploring;
        return;
    }

    // BUG TROUVE ET CORRIGE (inventaire inaccessible en combat, signale
    // par Jicehel -- impossible de boire une potion de soin en plein
    // combat) : actionC n'etait verifie que dans updateExploring(),
    // jamais ici. modeBeforeInventory permet de revenir au COMBAT en
    // cours (pas a l'exploration) en fermant l'inventaire.
    if ( input.justPressed( &InputState::actionC ) ) {
        modeBeforeInventory = ScreenMode::Combat;
        mode = ScreenMode::Inventory;
        return;
    }

    Enemy& enemy = enemies[activeEnemyIndex];
    bool actionEdge = input.justPressed( &InputState::actionA );

    FightState before = enemy.getFightState();
    enemy.update( actionEdge );

    // BUG TROUVE ET CORRIGE (bande du bas jamais mise a jour pendant le
    // combat, signale par Jicehel) : la bande de description affichait
    // toujours le nom du niveau, jamais "YOU HIT X: 4DMG"/"X HITS: 4DMG"/
    // "YOU KILLED X" comme l'original (Enemy.pde). "4DMG" litteral des
    // deux cotes est voulu, pas une erreur -- verifie dans le vrai code
    // source (lastSentence="...: 4DMG" en dur, quels que soient les PV
    // reellement retires).
    if ( lastSeenFightState != FightState::EnemyAttacking && enemy.getFightState() == FightState::EnemyAttacking ) {
        player.applyDamage( 1 );
        char buf[48];
        std::snprintf( buf, sizeof( buf ), translator.translate( "CBT_ENEMY_HITS" ), enemy.getDescription() );
        combatMessage = buf;
    }
    if ( before == FightState::EnemyAttacking && enemy.getFightState() == FightState::PlayerAttacking ) {
        char buf[48];
        std::snprintf( buf, sizeof( buf ), translator.translate( "CBT_YOU_HIT" ), enemy.getDescription() );
        combatMessage = buf;
    }
    lastSeenFightState = enemy.getFightState();

    if ( enemy.isDefeated() ) {
        char buf[48];
        std::snprintf( buf, sizeof( buf ), translator.translate( "CBT_YOU_KILLED" ), enemy.getDescription() );
        combatMessage = buf;
    }

    if ( enemy.isDefeated() && actionEdge ) {
        dialogue.showWinFight( enemy.getDescription(), 10 );
        modeBeforeDialogue = ScreenMode::Exploring;
        mode = ScreenMode::DialogueView;
    }
}

void GameApp::updateInventory( const IInput& input ) {
    if ( input.justPressed( &InputState::actionB ) ) { mode = modeBeforeInventory; return; }

    if ( inventory.getActiveTab() == InventoryTab::Items ) {
        if ( input.justPressed( &InputState::right ) || input.justPressed( &InputState::down ) ) inventory.selectNext();
        if ( input.justPressed( &InputState::left )  || input.justPressed( &InputState::up ) )    inventory.selectPrev();

        if ( input.justPressed( &InputState::actionA ) && !inventory.isEmpty() ) {
            InventoryEntry used;
            if ( inventory.takeSelected( used ) ) {
                if ( used.type == ObjectType::Potion && player.getCurrentHP() < player.getMaxHP() ) {
                    player.heal( 20 );
                }
            }
        }
    }

    if ( input.justPressed( &InputState::l1 ) || input.justPressed( &InputState::r1 ) ) inventory.switchTab();
}

void GameApp::updateDialogue( const IInput& input ) {
    switch ( dialogue.getKind() ) {
        case DialogueKind::Loot:
            if ( input.justPressed( &InputState::actionA ) ) {
                if ( pendingLootObjectIndex >= 0 && pendingLootObjectIndex < (int)objects.size() ) {
                    inventory.addItem( dialogue.getLootType(), dialogue.getItemName() );
                    objects[pendingLootObjectIndex].setActive( false );
                }
                pendingLootObjectIndex = -1;
                dialogue.close();
                mode = modeBeforeDialogue;
            } else if ( input.justPressed( &InputState::actionB ) ) {
                pendingLootObjectIndex = -1;
                dialogue.close();
                mode = modeBeforeDialogue;
            }
            break;

        case DialogueKind::WinFight:
            if ( input.justPressed( &InputState::actionA ) ) {
                player.gainXP( dialogue.getXpAwarded() );
                if ( activeEnemyIndex >= 0 && activeEnemyIndex < (int)enemies.size() )
                    enemies.erase( enemies.begin() + activeEnemyIndex );
                activeEnemyIndex = -1;
                dialogue.close();
                mode = modeBeforeDialogue;
            }
            break;

        case DialogueKind::None:
            mode = modeBeforeDialogue;
            break;
    }
}

const char* GameApp::descriptionKey() const {
    // Texte permanent en bas d'ecran, toujours visible (comme l'original :
    // text(description, 50*multi, 78*multi), dessine en dernier par
    // dessus tout) -- oublie dans les sessions precedentes. Niveau =
    // description brute du niveau (pas de traduction -- nom propre,
    // comme "UNDERGROUND LAIR"), le reste passe par translate().
    switch ( mode ) {
        case ScreenMode::MinimapView: return translator.translate( "DESC_MINIMAP" );
        case ScreenMode::Inventory:
            return translator.translate( inventory.getActiveTab() == InventoryTab::Stats ? "DESC_STATS" : "DESC_INVENTORY" );
        case ScreenMode::DialogueView:
            return translator.translate( dialogue.getKind() == DialogueKind::WinFight ? "DESC_YOU_WON" : "DESC_LOOTING" );
        case ScreenMode::Combat:
            return combatMessage.c_str();
        case ScreenMode::Exploring:
        default:
            return level.getDescription();
    }
}

void GameApp::render( IRenderer& renderer ) {
    // BUG TROUVE ET CORRIGE (texte residuel non efface, signale par
    // Jicehel) : l'original appelle "background(0)" EN PREMIER, a
    // CHAQUE frame, sans exception (darkUnderCOLOR.pde::draw()) --
    // aucun equivalent n'existait ici. Sans ca, une zone qui n'est pas
    // entierement recouverte par du contenu opaque a chaque frame (le
    // texte de description en bas, par exemple, dont la largeur change
    // d'un message a l'autre) garde les pixels de la frame precedente.
    renderer.fillRect( 0, 0, 150 * 2, 81 * 2, RGBColor{ 0, 0, 0 } );

    if ( mode == ScreenMode::Splash ) {
        splash.render( renderer, translator );
        renderer.present();
        return;
    }

    // BUG TROUVE ET CORRIGE (retour de Jicehel, avec captures d'ecran du
    // jeu d'origine a l'appui) : Vision+UI ne sont TOUJOURS dessines
    // qu'en Exploring/Combat dans la version precedente -- alors que la
    // vraie boucle draw() de l'original (darkUnderCOLOR.pde) les dessine
    // EN PERMANENCE, quel que soit l'ecran actif (sauf splash), et
    // superpose Minimap/Dialogue/Inventory/Stats PAR-DESSUS, pas a leur
    // place. C'est cette couche manquante qui donnait l'impression d'un
    // parchemin "trop petit" a chaque test -- le parchemin (a sa vraie
    // taille d'origine, 300x162, JAMAIS agrandi dans le code source)
    // etait en fait deja a la bonne taille tout du long ; ses marges
    // transparentes laissent voir le panneau de droite du dessous, qui
    // n'existait tout simplement pas dans les rendus precedents.
    ui.renderFrame( renderer );
    vision.render( renderer, level, player, objects, enemies );
    ui.renderStatus( renderer, player, vision.getDirection() );
    if ( mode == ScreenMode::Combat && activeEnemyIndex >= 0 && activeEnemyIndex < (int)enemies.size() ) {
        ui.renderCombat( renderer, enemies[activeEnemyIndex] );
    }

    switch ( mode ) {
        case ScreenMode::MinimapView:
            minimap.render( renderer, level, player, vision.getDirection(), objects, enemies );
            break;

        case ScreenMode::Inventory:
            if ( inventory.getActiveTab() == InventoryTab::Stats ) stats.render( renderer, player, translator );
            else inventory.render( renderer, player, translator );
            break;

        case ScreenMode::DialogueView:
            dialogue.render( renderer, translator );
            break;

        case ScreenMode::Exploring:
        case ScreenMode::Combat:
        case ScreenMode::Splash:
            break; // deja traites plus haut / rien de plus a superposer
    }

    // Texte de description permanent, toujours dessine EN DERNIER, par
    // dessus tout le reste (meme ordre que l'original : dernier appel
    // de draw()). Centre horizontalement sur l'ecran entier (pas sur le
    // parchemin -- l'original le centre bien sur 50*multi, soit le
    // milieu du canevas 150 de large, panneau de droite inclus).
    const char* desc = descriptionKey();
    // BUG TROUVE ET CORRIGE (2e passe, mesures precises de Jicehel) :
    // premiere estimation (x=100, y=144) encore fausse -- decalee de
    // 42px vers la gauche (7 caracteres x 6px, police Narrow) et 2px
    // vers le bas.
    renderer.drawText( 50 * 2 - 42, 78 * 2 - 10, desc, RGBColor{ 0xff, 0xff, 0xff }, FontSize::Narrow );

    renderer.present();
}
