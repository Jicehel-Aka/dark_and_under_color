# Dark & Under (couleur) -> AKA + PC/SDL — Journal de développement

**Voir [`README.md`](README.md) pour la présentation du projet, les
instructions de compilation et les commandes.** Ce fichier-ci est le
journal détaillé, session par session, de tout le travail de portage —
utile pour comprendre POURQUOI une décision a été prise ou un bug
corrigé d'une certaine façon, pas pour une prise en main rapide.

Portage du remake couleur (Processing/Java, fourni par Jicehel avec
l'autorisation de l'auteur — voir `LICENSE` pour le détail complet des
droits, y compris les ressources tierces (musique, police)) vers deux
cibles : AKA (ESP32-S3) et PC (SDL2, pour tester sans matériel). Aucune
couche de compatibilité type Arduboy2 ici -- Processing n'a aucun
rapport avec cette API, portage direct en C++ natif par-dessus
`gb_graphics`/`gb_core` (AKA) ou SDL2 (PC).

**État actuel : le jeu tourne réellement sur les deux cibles** (testé
sur matériel AKA et en exécutable PC, pas seulement en compilation) --
voir les sessions les plus récentes ci-dessous pour le détail des
derniers ajustements. Les toutes premières sessions listées plus bas
ne reflètent que l'état de CE moment-là (uniquement vérifié par
compilation, jamais exécuté) -- gardées telles quelles comme trace
historique, pas comme description de l'état actuel.

## Architecture

- `shared/platform/` — `IRenderer`/`IInput`/`Time.h` : les seules
  interfaces que le code de jeu connaît. Deux implémentations
  concrètes : `platform_sdl/` (charge les PNG à chaud) et
  `platform_aka/` (buffers RGB565 précompilés, voir `tools/`).
- `shared/game/` — toute la logique de jeu, indépendante de la
  plateforme : `entities/` (Object, Player, Enemy), `levels/`
  (TileType, LevelData, Level, Level00 — données du niveau séparées du
  rendu, contrairement à l'original), `rendering/` (Vision, Minimap,
  Stats, Splash), `GameApp` (assemble tout, remplace les globales +
  `draw()` de l'original), `RelativeMovement.h`.
- `tools/convert_assets.py` — PNG -> tableaux `uint16_t` RGB565 pour
  AKA, transparence par couleur-clé (0xF81F, confirmé dans le vrai code
  de `gb_graphics_image.cpp` : pas de canal alpha réel).

## Ce qui est porté et compile

- **Object, Player, Enemy** — logique complète (déplacement, collision,
  combat). Callbacks au lieu de variables globales (découplage demandé).
- **Level/LevelData/TileType** — grille du niveau 00 transcrite,
  `TileType::Special` (valeur 2) identifié comme une variante de
  texture de mur décorative (pas une porte comme supposé au départ —
  clarifié en portant Vision).
- **Vision** — rendu du couloir. RÉÉCRITURE MAJEURE par rapport à
  l'original : les 4 fonctions quasi-identiques `lookForward/Right/
  Back/Left` (~350 lignes dupliquées 4 fois) remplacées par UNE fonction
  générique paramétrée par direction (vecteur avant/droite), vérifiée
  mathématiquement case par case contre les 4 originales avant
  remplacement. Objets/ennemis visibles dans l'axe de vue, jusqu'à 2
  cases, avec occlusion simple (case juste devant).
- **Minimap, Stats, Splash** — portés fidèlement (fenêtre 9x9 autour du
  joueur, stats du héros, écran titre/intro/crédits). Texte d'intro
  identique à l'original (fichier fourni par Jicehel).
- **RelativeMovement.h** — la table de `Buttons.pde` qui traduit
  avancer/reculer/gauche/droite RELATIFS à la direction de vue en
  déplacement absolu sur la grille (pas juste haut/bas/gauche/droite au
  sens brut) — trouvée en lisant `Buttons.pde`, cruciale pour que les
  déplacements soient corrects.
- **GameApp** — assemble tout, gère les transitions Splash -> Explore ->
  Combat -> Minimap/Stats.
- **Assets** — 55 images converties en RGB565 (murs, mini-carte, stats,
  splash, objets, icônes ennemis de couloir), ~420 Ko en flash.

## Ce qui N'EST PAS encore porté

- **Inventory.pde, Dialogue.pde, UI.pde, Buttons.pde** — lus et compris
  (voir le code source d'origine), mais pas traduits en C++. Tous les
  quatre reposent sur des interactions à la SOURIS (`mousePressed`,
  survol `mouseX`/`mouseY`) — à repenser entièrement pour un jeu au
  D-pad/boutons avant de les porter, pas une simple traduction ligne à
  ligne comme le reste.
- **Rendu de l'écran de combat** (`Enemy::display()`/`enemyAttack()`/
  `playerAttack()` de l'original) — la LOGIQUE de combat est portée
  (`Enemy::update()`), mais son AFFICHAGE (barre de vie, sprite animé,
  effets de coup) n'est pas câblé dans `GameApp::render()`.
- **UI permanente en jeu** (barres PV/XP, boussole, boutons d'action) —
  `UI.pde` non porté, donc rien de tout ça ne s'affiche pendant
  l'exploration pour l'instant.
- **Bug connu, pas corrigé** : `GameApp::updateCombat()` va appliquer
  les dégâts au joueur à CHAQUE FRAME tant que l'ennemi attaque, pas une
  fois par assaut (`doOnce` de l'original pas encore câblé côté
  GameApp — la logique existe dans `Enemy` mais n'est pas exploitée).
  Marqué TODO explicite dans le code.
- **Feuilles de sprites d'ennemis** (`ENM_rat.png`/`ENM_skelly.png`,
  animation 3 frames) — pas encore converties (seulement les icônes
  moyenne/petite utilisées dans le couloir).
- **Rotation du regard** (tourner gauche/droite sans se déplacer) —
  callback pas encore branché sur un bouton dans `GameApp::updateExploring()`.

## Prochaines étapes suggérées

1. Décider d'un mapping de boutons AKA/SDL pour les 6 actions
   (avancer/reculer/strafe gauche-droite/tourner gauche-droite/action) —
   le D-pad à 4 directions ne suffit plus une fois qu'on ajoute la
   rotation, contrairement à Dark & Under 1.
2. Porter `UI.pde` (barres, boussole) — le plus utile en premier, visible
   en permanence pendant l'exploration.
3. Repenser Inventory/Dialogue/Buttons pour une interaction bouton
   plutôt que souris.
4. Câbler l'affichage du combat dans `GameApp`.
5. Compilation réelle sur le toolchain ESP-IDF (jamais testée) et build
   SDL réelle (CMake + SDL2 + SDL2_image, jamais linkée pour de vrai
   faute de dépendances système complètes dans ce bac à sable).

## Session 2 : boutons (plus de souris) + inventaire/dialogue

Schéma de boutons validé avec Jicehel :
- **D-pad** : avancer/reculer/strafe gauche-droite, RELATIFS à la
  direction de vue (comme l'original couleur, pas comme Dark & Under 1).
- **L1/R1** : tournent le regard en exploration ; changent d'onglet
  Items/Stats une fois l'inventaire ouvert (pas de conflit, on ne se
  déplace pas dans un menu).
- **A** : valider/attaquer/utiliser/accepter. **B** : retour/refuser.
- **C** : ouvre l'inventaire. **D** : bascule la mini-carte.
- **Stats** n'a plus d'écran séparé (aucun bouton physique restant) —
  devient un second onglet de l'inventaire, décision prise sans
  reconsulter Jicehel (justifiée dans le code).

**Porté** : `Inventory` (navigation D-pad, A=utiliser, B=jeter, tabs
Items/Stats), `Dialogue` (butin trouvé + victoire de combat, A/B au
lieu du survol souris), câblage complet dans `GameApp` — les
déclenchements butin/combat, avant simplement notés en TODO, sont
maintenant réellement fonctionnels (`checkTriggers()`,
`updateDialogue()`).

**Bug de l'original réellement corrigé cette fois** (pas juste signalé) :
les dégâts au joueur pendant le combat s'appliquaient à chaque frame
tant que le bouton restait enfoncé (`doOnce` de l'original, jamais câblé
côté `GameApp` en session 1). Réglé en s'appuyant sur
`IInput::justPressed()` (détection de front, déjà disponible) plutôt que
sur l'état brut du bouton.

**Toujours pas fait** : `UI.pde` (barres PV/XP et boussole visibles en
permanence pendant l'exploration), affichage détaillé du combat (sprite
animé, barre de vie de l'ennemi — la logique existe dans `Enemy` mais
rien ne l'affiche encore), les 5 fichiers de langue du jeu (dialogues,
noms d'objets, intro) — seuls fr/en prévus pour l'instant par choix de
Jicehel, de/es/it repoussés à plus tard. Rendu Inventory encore
minimal (sélection marquée par du texte "^", pas par l'image de
surbrillance `BUT_inventoryOver.png` de l'original).

## Session 3 : UI permanente + affichage du combat

**Porté** : `UI` (barres PV/XP + boussole, visibles en permanence
pendant l'exploration ET le combat ; sprite animé de l'ennemi, barre de
vie, effets de coup pendant le combat). Les boutons tactiles de
l'original (avancer/tourner/inventaire dessinés en icônes cliquables)
n'ont pas été repris -- inutiles, remplacés par de vrais boutons
physiques (D-pad/L1/R1/C/D, voir session 2).

Ajout technique nécessaire : `IRenderer::drawImageRegionScaled()` --
absent jusqu'ici, nécessaire pour découper une feuille de sprites
(boussole 4 cadrans, ennemis 3 frames d'animation) sans dupliquer les
PNG en 4 ou 3 fichiers séparés. Implémenté des deux côtés (SDL :
`SDL_Rect` source ; AKA : la surcharge `gb_graphics::drawImageScaled`
avec région source, déjà présente dans la bibliothèque enrichie pour
Dark & Under 1).

Bug trouvé et corrigé pendant cette passe (pas dans l'original --
introduit par moi puis repéré au compile) : `EnemyType` oublié dans les
déclarations anticipées de `UI.h`, pris pour un `int` par le
compilateur.

64 images converties au total (~520 Ko en flash).

**Toujours pas fait** : les fichiers de langue fr/en (dialogues, noms
d'objets, intro traduits), le rendu de sélection dans l'inventaire reste
un texte "^" plutôt que l'image de surbrillance d'origine, pas de test
réel sur matériel ni build SDL exécutée pour de vrai (toujours limité à
la vérification de syntaxe dans ce bac à sable).

## Session 4 : traduction fr/en

Réutilisé le système existant plutôt que d'en inventer un : `ITranslator`
(interface commune), `AkaTranslator` (enveloppe autour d'`AkaRuntime::
translate()`, qui charge déjà `/sdcard/AKA/lang/*.json` PUIS
`/sdcard/DarkUnderColor/lang/*.json` -- même mécanisme que Dark & Under
1), `SdlTranslator` (mini-analyseur JSON plat maison pour la build PC,
même format que les fichiers AKA).

Toutes les chaînes en dur repérées dans `Dialogue`/`Inventory`/`Stats`/
`Splash` remplacées par des clés (`DLG_*`, `INV_*`, `ITEM_*`,
`STATS_HERO_NAME`, `SPLASH_INTRO_1..6`) -- `GameApp` porte maintenant
une référence `ITranslator&`, transmise à chaque `render()` qui en a
besoin. `main.cpp` (AKA) appelle aussi `akaRuntime.setControlsKeys()`
avec des clés `CTRL_*` dédiées (jamais fait dans les sessions
précédentes) -- nécessaires pour que l'écran "Commandes" du menu système
affiche quelque chose de correct pour ce jeu.

`sdcard_files/DarkUnderColor/lang/{fr,en}.json` écrits avec le contenu
réel (texte d'intro traduit en français, pas de "TODO"). de/es/it
repoussés à plus tard, comme convenu.

**Toujours pas fait** : le rendu de sélection dans l'inventaire (texte
"^" provisoire), aucun test réel sur matériel ni build SDL exécutée
pour de vrai (toujours limité à la vérification de syntaxe).

## Session 5 : corrections demandées par Jicehel

- **Splash corrigé** : l'écran-titre a bien maintenant DEUX boutons
  (A=JOUER lance l'intro, B=CREDITS revient à l'écran des crédits),
  comme l'original -- la première version avançait en boucle linéaire
  sur un seul bouton, une simplification jamais signalée clairement.
  Bug trouvé au passage en le corrigeant : le code lisait l'état BRUT
  du bouton (`s.actionA`) plutôt que la détection de front
  (`justPressed`) -- en le maintenant enfoncé, tous les écrans
  auraient défilé en une frame. Corrigé avec le même mécanisme que le
  combat (session 2).
- **Image de surbrillance d'inventaire** : `BUT_inventoryOver.png`
  (16x16, converti et câblé) remplace le texte "^" provisoire.
  65 images au total désormais, ~520 Ko en flash.
- **Musique** : PAS implémentée -- à clarifier avec Jicehel avant de
  s'y mettre (voir échange en conversation, question de licence sur la
  piste tierce utilisée par l'original).

## Session 6 : premier vrai build ESP-IDF chez Jicehel -- 2 bugs de structure trouvés

Premier retour de compilation réelle (pas juste `-fsyntax-only`) :
`undefined reference to 'app_main'` à l'édition de liens, et la cible
compilée était l'ESP32 classique (`xtensa-esp32-elf-g++`) au lieu de
l'ESP32-S3 (`xtensa-esp32s3-elf-g++`). Deux oublis de ma part, pas des
bugs dans le code lui-même :

1. **Aucun `sdkconfig`/`sdkconfig.defaults` n'existait pour ce projet**
   (contrairement à `dark_and_under_aka`, où ça avait été fait) -- sans
   `CONFIG_IDF_TARGET="esp32s3"`, le build est parti sur la cible par
   défaut (ESP32 classique). Ajoutés : `sdkconfig.defaults`,
   `partitions.csv` (même schéma que `dark_and_under_aka`), et un
   `sdkconfig` complet copié depuis ce même projet de référence (même
   cible matérielle exacte, même version ESP-IDF 5.5.1).

2. **Le point d'entrée (`app_main`) vivait dans `platform_aka/`, un
   composant ORDINAIRE** -- ESP-IDF ne lie automatiquement dans le
   binaire final que le composant nommé EXACTEMENT `main` ; rien ne
   dépendant de `platform_aka`, ses fichiers (dont `app_main`) étaient
   tout simplement absents du lien. Restructuré :
   - `main/` : seulement `main.cpp` + son `CMakeLists.txt`
     (`REQUIRES gamebuino aka_runtime platform_aka`).
   - `components/platform_aka/` : `AkaRenderer`/`AkaInput`/
     `AkaTranslator`/`GeneratedAssets.cpp` + toute la logique de jeu
     partagée (chemins relatifs vers `shared/` ajustés d'un niveau,
     le composant étant maintenant sous `components/`).

Tout revérifié après restructuration (syntaxe complète, tous fichiers) --
propre. Root cause identifiée par lecture directe du message d'erreur
(nom du compilateur invoqué, symbole manquant), pas par supposition.

## Session 7 : -Werror=maybe-uninitialized

Deuxième retour de build réel : `'playerMarker' may be used
uninitialized` dans `Minimap.cpp`, promu en erreur par `-Werror` --
GCC ne peut pas prouver qu'un `switch` sur les 4 seules valeurs de
`FacingDirection` (sans `default`) couvre tous les cas, même si c'est
vrai en pratique. Trois occurrences du même schéma trouvées et
corrigées par une valeur par défaut avant le switch (`Minimap.cpp` +
deux dans `Vision.cpp`, `renderObjects`/`renderEnemies`) -- plus sûr
qu'un simple silence de l'avertissement. Une erreur d'édition de ma
part au passage (ligne `switch` supprimée par accident) repérée et
corrigée avant de continuer.

Cause de fond : contrairement à `dark_and_under_aka`, ce projet
n'avait JAMAIS reçu la liste de tolérances `-Wno-error=...` sur ses
`CMakeLists.txt` -- ajoutée maintenant à `components/platform_aka/`.
Revérifié avec les mêmes options strictes que celles qui ont fait
échouer le build (`-Wall -Wextra -Werror=maybe-uninitialized`), pas
seulement `-fsyntax-only` par défaut.

## Session 8 : undefined reference to 'kLevel00'

Troisième retour de build réel -- bonne nouvelle au passage : la cible
est maintenant correctement ESP32-S3 et le fix `main/` fonctionne (le
lien va beaucoup plus loin, 1100/1103). Nouvelle erreur, cette fois
côté C++ pur : `undefined reference to 'kLevel00'`.

**Cause** : une variable globale `const` a une liaison INTERNE par
défaut en C++ (contrairement au C), même si sa DÉCLARATION dans un
en-tête est marquée `extern` (`Level.h` déclarait bien
`extern const LevelData kLevel00;`) -- c'est la DÉFINITION elle-même
qui doit porter `extern` pour que le symbole soit visible depuis un
autre fichier `.cpp`. `Level00.cpp` définissait `const LevelData
kLevel00 = {...}` sans ce mot-clé : chaque fichier consommateur
(`main.cpp`) croyait qu'un symbole externe existait, mais l'unique
définition réelle restait invisible au lien.

**Corrigé** : `extern const LevelData kLevel00 = {...}` dans
`Level00.cpp`. Vérifié au-delà de la simple syntaxe cette fois :
compilation séparée de `Level00.cpp` et d'un petit programme de test
qui consomme `kLevel00` depuis un autre fichier, puis édition de liens
réelle des deux `.o` -- reproduit fidèlement le scénario multi-fichiers
qui avait échoué, pas juste un `-fsyntax-only` sur un seul fichier.

Balayé le reste du code pour le même piège (`extern const` déclaré
dans un en-tête mais défini sans `extern` ailleurs) : aucun autre cas,
seul `kLevel00` était concerné.

## Session 9 : premier retour visuel réel -- 4 bugs trouvés et corrigés

Premier retour de Jicehel après avoir vu le jeu tourner à l'écran (pas
juste compiler) -- quatre problèmes, tous corrigés :

1. **Rouge/bleu inversés** (le logo Garage Collective orange
   apparaissait bleu). Cause : le vrai `gb_graphics` (`lcd_color_rgb`,
   `gb_ll_lcd.h`) range ROUGE dans les bits BAS et BLEU dans les bits
   HAUTS -- l'inverse du RGB565 "standard", malgré le nom de la
   fonction. `tools/convert_assets.py` utilisait l'ordre standard.
   Formule corrigée (`(b>>3)<<11 | (g>>2)<<5 | (r>>3)`), les 65 assets
   régénérés avec les bonnes valeurs.

2. **Texte de dialogue invisible**. `AkaRenderer::drawText()` était un
   no-op laissé par erreur ("gb_graphics n'a pas de print_str()" --
   c'était faux, pas vérifié à temps). `gb_graphics.h` a bien
   `move_cursor()`/`print_str()`/`setColor()` -- câblé maintenant.

3. **Écran envahi de magenta, transparence absente**. Cause : les
   appels `gfx.drawImage()`/`drawImageScaled()` utilisaient la
   surcharge SANS couleur-clé -- chaque pixel "transparent" (magenta
   dans les données converties) était donc dessiné tel quel au lieu
   d'être sauté. Corrigé en utilisant systématiquement les surcharges
   AVEC couleur-clé (`0xF81F`, déjà présentes dans `gb_graphics.h`,
   jamais utilisées jusqu'ici). Côté SDL, correctif différent mais
   même symptôme visé : `SDL_SetTextureBlendMode(BLEND)` explicite sur
   chaque texture (les PNG ont un vrai canal alpha côté SDL, pas besoin
   de couleur-clé, juste d'activer le mélange).

4. **Affichage non centré**. Le viewport logique (300×162 après zoom
   x2) était collé en `(0,0)` sur l'écran AKA 320×240 au lieu d'être
   centré. Décalage `(10, 39)` ajouté dans `AkaRenderer` (appliqué à
   tous les appels de dessin). Côté SDL, aucun décalage ajouté : la
   fenêtre de test fait déjà exactement 300×162, pas de marge à gérer.

Tout revérifié après coup (syntaxe + régénération des assets) -- propre
des deux côtés.

**Toujours pas fait** : rendu de texte réel côté SDL (toujours un
rectangle-repère, pas du vrai texte -- `SDL_ttf` volontairement évité
jusqu'ici pour ne pas ajouter de dépendance système).

## Session 10 : quatre nouveaux retours après test réel

1. **Mini-carte débordant du parchemin.** Cause bien plus profonde
   qu'un simple mauvais chiffre : `kTileSize` valait 16, en confondant
   `TILE_SIZE = 4 * multi` de l'original (16 sur leur cible PC) avec sa
   valeur LOGIQUE (4, "multi"=4 étant déjà leur propre facteur
   d'échelle). Avec 16, chaque case de la grille 9x9 faisait 32px
   (16x2) au lieu de 8px -- 288px de hauteur totale contre 162px de
   parchemin disponible. Corrigé : `kTileSize = 4` (`Config.h`),
   vérifié directement contre `darkUnderCOLOR.pde` et `Player.pde`.
   Les positions de `Player`/`Object`/`Enemy` (jamais consommées par le
   rendu actuel, seuls les indices de grille comptent) ne sont pas
   affectées côté affichage par ce changement.

2. **Musique inaudible** -- confirmé, pas un bug : le son n'a jamais
   été implémenté dans ce portage (aucun code audio écrit à ce jour,
   voir échange précédent sur la question de licence). Rien à corriger
   ici, un vrai chantier à part entière.

3. **Menu système AKA / capture d'écran inopérants.** Le vrai bug
   caché derrière ce symptôme : `main.cpp` déclarait `Keys keys;` SANS
   JAMAIS appeler `input_poll()` avant de le passer à
   `akaRuntime.update(keys)` -- celui-ci lisait donc `k.MENU`/`k.RUN`
   sur de la mémoire de pile non initialisée à chaque frame,
   empêchant tout : ouverture du menu (volume, retour loader, langue,
   crédits), retour au loader (RUN+MENU) ET capture d'écran (MENU
   maintenu 500ms) sont pourtant TOUS déjà gérés automatiquement à
   l'intérieur d'`AkaRuntime::update()` (vérifié dans le vrai
   `aka_runtime.cpp`) -- rien à "ajouter", juste ce bug à corriger.
   Corrigé sur le modèle exact de `dark_and_under_aka`
   (`input_poll(keys)` avant `akaRuntime.update(keys)`).

4. **Texte du dialogue de butin mal centré.** Les positions étaient
   des valeurs fixes ignorant la longueur réelle du texte -- forcément
   faux dès que la traduction change de longueur (français vs anglais).
   Police confirmée fixe 8px/caractère (`font8x8_basic.h`) : centrage
   réel calculé à partir de `strlen(texte) * 8`, plus une position en
   dur. S'applique aussi à l'écran de victoire de combat (même défaut).

Tout revérifié après coup, propre.

## Session 11 : centrage sur le vrai parchemin + textes trop longs

**Cause de fond du décalage icône/texte** (potion, écran de victoire) :
le parchemin (`UI_parchemin.png`, 150x81) n'occupe pas tout son canevas
-- sa zone visible réelle mesure ~62x56px logiques, centrée vers
`(49, 36)`, pas `(75, 40)` comme le centre géométrique de l'image le
laissait supposer. Vérifié en mesurant la vraie boîte englobante du
contenu non-transparent (Python/PIL), pas deviné. `Dialogue.cpp`
recalculé pour centrer sur ce vrai centre (`kParchmentCenterX = 98` en
unités doublées) plutôt que sur la largeur totale de l'écran/canevas.
Ligne "+X XP" aussi décalée de 10px vers le haut, comme demandé.

**Police plus étroite demandée -- pas possible** : `gb_graphics` n'a
qu'UNE seule police, fixe 8px/caractère (`font8x8_basic.h`), aucune
alternative plus étroite dans la bibliothèque. Compensé en raccourcissant
les textes français trop longs pour la largeur réelle du parchemin
(~15-16 caractères max) : "VOUS AVEZ TROUVE" -> "VOUS TROUVEZ",
"A: PRENDRE   B: LAISSER" -> "A:GARDE B:NON", "A: UTILISER   B: JETER"
-> "A:PREND B:JETE".

**Menu Commandes** : "Fleches" retiré du début de `CTRL_MOVE`/
`CTRL_STRAFE`, comme demandé.

**`MENU_LEGEND`/`MENU_VOLUME` non traduits** : même cause et même
solution que pour Dark & Under 1 -- ce sont des clés COMMUNES
(`/sdcard/AKA/lang/*.json`), absentes du fichier spécifique à ce jeu.
Ajoutées à `sdcard_files/DarkUnderColor/lang/{fr,en}.json`
(`MENU_LEGEND`, `MENU_VOLUME`, `MENU_VOL_MUSIC`, `MENU_VOL_SFX`) --
fonctionne car `translate()` fusionne les deux fichiers chargés en une
seule table, peu importe lequel contient la clé.

Stats/Inventaire volontairement PAS retouchés cette session (pas de
problème signalé dessus, pas de raison d'introduire un changement non
testé).

## Session 12 : musique implémentée

**Point de licence clarifié** : les métadonnées du MP3 fourni
(`Visager_-_15_-_Pyramid_Level_Loop.mp3`) révèlent qu'il s'agit d'une
piste tierce -- "Pyramid Level" par Visager, album *Songs From An
Unmade World*, hébergée sur Free Music Archive, sous licence Creative
Commons Attribution 4.0 (CC BY 4.0). Pas une composition de l'auteur du
jeu couleur lui-même comme d'abord indiqué (malentendu clarifié avec
Jicehel). CC BY 4.0 autorise cette réutilisation à condition de créditer
l'auteur -- attribution ajoutée au fichier `LICENSE`.

**Implémentation technique** : `gb_audio_track_wav::play_wav()` lit un
fichier `.wav` DIRECTEMENT depuis la carte SD (pas de conversion en
tableau C comme pour les images -- confirmé dans `gb_audio_track_wav.h`).
Format exigé confirmé dans le code : 44,1kHz, mono, 16 bits. Conversion
faite avec `ffmpeg` :
`sdcard_files/DarkUnderColor/music/level_loop.wav` (9,7 Mo).

`main.cpp` (AKA) complété : `gb_audio_player` + `gb_audio_track_wav`
globaux, tâche FreeRTOS dédiée (`audio_mix_task`, même schéma que
`poa_aka`) qui appelle `pool()` en boucle et relance la piste dès
qu'elle se termine (`gb_audio_track_wav` n'a pas d'option de bouclage
native). Volume musique du menu système AKA câblé sur
`set_master_volume()` via `akaRuntime.setVolumeChangedCallback()`.

**Pas encore fait** : pas de son côté SDL (chargement/lecture audio à
implémenter séparément si besoin de tester le son sur PC -- SDL_mixer
non ajouté pour l'instant). Pas de SFX (bruitages), seulement la
musique de fond.

## Session 13 : police plus étroite (Simple 5x8)

Jicehel a fourni une police pixel 5x8 ("Simple 5x8" par Atom596,
1001fonts.com) pour remplacer la police 8x8 par défaut, trop large.
Licence vérifiée directement sur la page du site avant intégration :
1001Fonts Free For Commercial Use (FFC), réutilisation libre.

**Implémentation** : `tools/` (script Python ponctuel, pas versionné --
utilise Pillow pour rasteriser chaque glyphe à 9pt, taille trouvée
empiriquement pour donner des cellules 5x8 pixels pile) génère
`components/platform_aka/simple5x8_font.h` : 95 glyphes (ASCII
imprimable 32-126), même format que `font8x8_basic.h` (un octet par
ligne, bit0 = colonne la plus à gauche) pour rester cohérent avec le
reste du code. Bug trouvé et corrigé PENDANT la génération (pas après) :
un premier essai laissait la ponctuation étroite (`:`, `.`) collée au
bord gauche de sa cellule au lieu d'être centrée -- corrigé en
recentrant chaque glyphe sur sa propre boîte encrée avant de le figer
dans la grille 5px, plutôt que de rendre brut à x=0.

`AkaRenderer::drawText()` réécrit pour dessiner ces glyphes
pixel-par-pixel (`drawPixel`) au lieu d'appeler `gfx.print_str()` --
avancement resserré à 6px/caractère (5 de glyphe + 1 d'espacement) au
lieu de 8px. **Ne touche QUE le texte dessiné par le jeu** (Dialogue/
Stats/Inventaire/Splash) -- le menu système AKA garde sa police 8x8
d'origine, dessinée directement par `aka_runtime.cpp` sans passer par
ici, volontairement pas modifié. `Dialogue.cpp` mis à jour
(`kCharWidthPx` 8->6) pour que le centrage sur le parchemin utilise la
bonne largeur.

Police créditée dans `LICENSE` (asset tiers, comme la musique).

**Pas encore fait** : police toujours 8x8 par défaut côté build SDL de
test (le placeholder rectangle-repère de `SdlRenderer::drawText()` n'a
pas été remplacé -- pas de vrai rendu de texte du tout côté SDL pour
l'instant, narrow ou pas).

## Session 14 : build SDL à niveau (texte réel + audio)

Pendant que Jicehel teste la build AKA, mise à niveau de la build PC/SDL
sur les deux manques identifiés en session 13 :

**Texte réel** : le rectangle-repère de `SdlRenderer::drawText()`
remplacé par un vrai rendu de glyphes, réutilisant directement les
mêmes données que la build AKA (`shared/platform/fonts/simple5x8_font.h`,
déplacé de `components/platform_aka/` vers un emplacement commun aux
deux plateformes -- une seule police à maintenir, cohérence visuelle
garantie). Dessiné pixel par pixel via `SDL_RenderDrawPoint`, pas de
dépendance `SDL_ttf` ajoutée.

**Audio** : `SDL2_mixer` intégré (`Mix_OpenAudio`/`Mix_LoadMUS`/
`Mix_PlayMusic`), même fichier `.wav` que la build AKA. Contrairement à
`gb_audio_track_wav` (AKA), `Mix_PlayMusic()` boucle nativement
(paramètre `-1`) -- pas besoin du redémarrage manuel utilisé côté AKA.
Musique simplement désactivée (pas bloquante) si l'ouverture audio
échoue, ex. pas de périphérique son disponible.

**Limite de ce bac à sable, pas du code** : `SDL2_mixer`, comme `SDL2`/
`SDL2_image` avant lui, a des dépendances système manquantes ici
(mêmes soucis de miroir apt rencontrés dès la toute première session
SDL) -- impossible de le lier réellement pour un vrai test d'exécution.
Vérifié uniquement par compilation de syntaxe avec un stub. À confirmer
par un vrai `cmake --build` chez Jicehel, où l'installation de
`SDL2_mixer` devrait être normale.

## Session 15 : retours après premier vrai test sur matériel AKA

Quatre points, tous corrigés :

1. **Pas de musique** -- vrai bug, pas un souci de fichier/format.
   `g_audio_player.set_master_volume()` n'etait appele QUE depuis le
   callback `onVolumeChanged()` (declenche seulement si l'utilisateur
   ouvre le menu et touche au volume) -- au demarrage, le volume
   maitre restait a sa valeur par defaut (silence), meme si la piste
   jouait reellement. Corrige en appliquant `akaRuntime.getMusicVolume()`
   explicitement avant `play_wav()`.

2. **Police du dialogue** -- retour de test : le 8x8 convient mieux a
   l'ecran de victoire (peu de texte, le 5x8 y "faisait vide"), le 5x8
   reste necessaire pour le dialogue de butin (plus de texte a faire
   tenir). `IRenderer::drawText()` prend maintenant un parametre
   `FontSize` (Wide/Narrow) -- `Dialogue.cpp` choisit Wide pour
   Victoire, Narrow pour Butin. Cote SDL, toujours une seule police
   disponible (5x8), le parametre est accepte mais ignore.

3. **Parchemin encore trop petit** (touchait les ombres du bord,
   "A:GARDE B:NON" en partie hors cadre) -- deuxieme elargissement,
   +10px supplementaires dans TOUS les sens. Recalcule depuis les
   300x162 d'origine (pas en ajoutant +10 sur les chiffres deja
   approximatifs de la 1ere passe) pour rester au plus pres d'une
   echelle uniforme : 350x190, origine (-25,-14), centre visuel du
   papier recalcule a (89, 70).

4. **Ecrans Splash/Credits assombris apres la conversion BGR565** --
   cause plus large que juste ces deux ecrans : la conversion utilisait
   ">>3"/">>2" (TRONQUE la partie fractionnaire) au lieu d'arrondir au
   plus proche -- biais systematique vers le bas sur TOUTE conversion 8
   bits -> 5/6 bits, juste plus visible sur ces grands aplats de
   couleur uniforme. Corrige pour TOUTES les images (`(r+4)>>3` au lieu
   de `r>>3`, meme principe pour vert/bleu). En plus de ce correctif,
   rehaussement explicite luminosite (+15%) et contraste (+12%)
   applique specifiquement aux 3 images Splash/Credits/Intro, comme
   demande. Les 65 assets regeneres.

Tout revérifié après coup (syntaxe complète, AKA et SDL), propre.

## Session 16 : trois nouveaux retours après test réel

**Musique toujours muette -- vrai bug trouvé cette fois (le fix de
volume de la session precedente etait correct mais insuffisant)** :
`gb_audio_track_wav::check_file_format()` (vrai code source lu
directement) exige que le chunk `data` suive IMMEDIATEMENT le chunk
`fmt ` (16 octets pile) dans le fichier WAV -- sans rien entre les
deux. La conversion ffmpeg d'origine embarquait des metadonnees
(les tags d'attribution CC BY reperes des la toute premiere
conversion) dans un chunk intermediaire, faisant echouer cette
verification stricte a chaque tentative de lecture -- silencieusement
du point de vue du joueur (erreur seulement visible sur la console
serie ESP32, jamais affichee dans le jeu). Reconverti avec
`-map_metadata -1 -fflags +bitexact` : structure de chunks verifiee
octet par octet (`RIFF`/`WAVE`/`fmt ` 16o/`data` immediat), correspond
maintenant exactement a ce qu'exige le code. Meme fichier resynchronise
vers `sdcard_files/` (AKA) et `platform_sdl/music/` (SDL).

**Splash/Credits toujours trop sombres -- rehaussement precedent
insuffisant, pas absent.** Mesure directe : luminosite moyenne
d'origine de `garCoLogo.png` ~45/40/30 sur 255 -- une image sombre par
conception, pas un artefact de conversion. Un "+15% lineaire" ne bouge
quasiment rien sur des valeurs deja basses. Remplace par une correction
GAMMA (1.8), bien plus efficace sur les tons sombres sans cramer les
hautes lumieres -- teste et mesure avant de choisir la valeur (moyenne
remontee a ~73/69/58, un vrai changement visible cette fois). Contraste
gardé, plus modeste (1.10 au lieu de 1.12) pour ne pas re-assombrir les
tons sombres deja releves par le gamma.

**Parchemin encore trop petit** -- troisieme passe d'elargissement,
+10px de plus dans tous les sens, toujours recalculee depuis les
300x162 d'origine (pas empilee sur les chiffres approximatifs des
passes precedentes) : 370x200, origine (-35,-19), centre visuel
recalcule a (86, 70).

Tout revérifié après coup (syntaxe complète, structure WAV verifiee
octet par octet, mesures de luminosite avant/apres), propre.

## Session 17 : correction architecturale -- Vision+UI doivent rester visibles sous tous les écrans

Jicehel a fourni 7 captures d'écran du jeu original en action. Elles
ont révélé une erreur de structure, pas un problème de taille comme
supposé pendant trois sessions de suite.

**La vraie boucle `draw()` de l'original** (`darkUnderCOLOR.pde`) :

```
background(0)
myVision.playerVision()    // TOUJOURS dessiné
if(!splash) myUI.display() // TOUJOURS dessiné (cadre + barres + boussole)
puis, PAR-DESSUS : minimap OU dialogue OU stats OU inventaire
```

Le couloir et le panneau de droite (PV/XP, compas, inventaire) ne sont
JAMAIS remplacés par la mini-carte/le dialogue/l'inventaire -- ces
derniers se dessinent EN SURIMPRESSION. Le parchemin
(`UI_parchemin.png`, 150x81) a des marges transparentes sur les côtés
(mesuré par bbox : contenu visible seulement x=[18,80]) qui laissent
voir le panneau de droite en dessous.

**Ce que ça change** : les trois agrandissements successifs du
parchemin (session 15, 16) corrigeaient un symptôme qui n'existait
pas -- `Dialogue.cpp` REVENU à sa taille d'origine exacte (300x162,
jamais modifiée dans le code source). La vraie cause du "parchemin
trop petit" a toujours été l'absence de la couche Vision+UI en dessous
des écrans Minimap/Dialogue/Inventory/Stats dans mon implémentation
précédente -- ils étaient traités comme des écrans de remplacement
plein écran, pas des surimpressions.

**Corrigé** : `GameApp::render()` restructuré pour dessiner Vision+UI
en base pour TOUS les modes sauf Splash (`ui.renderFrame()` avant
Vision, `ui.renderStatus()` après -- autre bug trouvé au passage : le
cadre se dessinait après Vision et le recouvrait), puis superposer
Minimap/Dialogue/Inventory/Stats par-dessus selon le mode actif -- même
composition en couches que l'original, pas juste un `switch` exclusif.
`UI.h`/`UI.cpp` séparés en `renderFrame()` (le cadre, à appeler avant
Vision) et `renderStatus()` (barres/boussole, à appeler après).

Deux erreurs d'édition de ma part trouvées et corrigées EN COURS de
cette restructuration (du code dupliqué laissé par un remplacement de
texte imprécis, accolades déséquilibrées) -- reperees par un contrôle
systématique de l'équilibre des accolades avant de continuer, pas
laissées pour un futur build.

Tout revérifié après coup : syntaxe complète, AKA et SDL, tous les
fichiers modifiés cette session.

## Session 18 : six retours après test + comparaison directe aux captures de référence

**1. Flèches de direction (D-pad) manquantes.** Retirées lors du
passage aux vrais boutons physiques, mais l'original les garde comme
repère visuel statique (non cliquable, juste affiché). Six icônes
reconverties (`BUT_forward/backwards/left/right/turnLeft/turnRight.png`)
et redessinées aux memes positions que l'original dans
`UI::renderStatus()`.

**2. Sol/plafond du couloir trop sombres.** Mesure directe : luminosite
moyenne de `dungeon_back1/2.png` ~21-27/255 -- plus sombre encore que
les ecrans Splash. Marque pour rehaussement gamma (voir point 6 pour la
methode).

**3. Cercle XP + signe "+".** Corrige pour matcher l'original exactement
(`Dialogue.pde` ne dessine que le nombre brut par-dessus l'icone, jamais
"+10 XP") : `+` retire, cercle agrandi (32x32 -> 36x36), nombre centre
sur l'icone au lieu d'etre affiche separement en dessous.

**4. Texte de description en bas d'ecran manquant.** Oublie depuis le
debut : l'original dessine un texte permanent
(`text(description,50*multi,78*multi)`) en TOUT DERNIER, par-dessus
tout, changeant selon l'ecran ("UNDERGROUND LAIR" en exploration,
"MINIMAP", "YOU WON"...). Ajoute (`GameApp::descriptionKey()` + nouvelles
cles de traduction `DESC_*`).

**5. Parchemin -- Jicehel confirme qu'on peut l'elargir vers la
droite/le bas sans souci** (maintenant que la vraie cause architecturale
de la session precedente est corrigee). Elargi de (300,162) a (320,182),
origine (0,0) inchangee -- pour donner un peu de place au cercle XP
agrandi.

**6. Assombrissement Splash/Credits -- corrige au MAUVAIS niveau,
vraie mesure de reference disponible cette fois.** Jicehel a fourni les
7 captures d'ecran d'origine utilisees pour la restructuration
architecturale (session 17) -- mesure directe de la luminosite des deux
premieres (titre, credits) contre les images sources AVANT tout
rehaussement :
- Credits (`garCoLogo.png`) : source brute ~46/40/30, cible mesuree sur
  la capture ~38/36/24 -- gamma reel necessaire = 0.901 (LEGER
  ASSOMBRISSEMENT, pas un eclaircissement !).
- Titre (`UI_titleScreen.png`) : source brute ~81/69/47, cible mesuree
  ~77/69/42 -- gamma reel = 0.958 (quasi neutre).

Le gamma=1.8 choisi "a l'oeil" en session 16 SURCORRIGEAIT largement
(sortie mesuree ~73/69/58, tres au-dessus de la vraie cible). Remplace
par gamma=0.93 (moyenne des deux mesures) pour Splash/Credits/Intro
specifiquement -- `VisionBack`/`VisionBack2` gardent un gamma distinct
(1.8, pas de reference contradictoire pour ceux-la, mesure de depart
bien plus sombre). `convert_assets.py` reecrit pour accepter un gamma
PAR IMAGE (plus un simple booleen global) -- 71 assets regeneres.

**Musique -- toujours pas certain, malgre une 3e investigation
approfondie.** Analyse du vrai `gb_audio_player.cpp` : le volume par
piste (`get_track_volume()`) est correctement a 1.0 par defaut (pas le
probleme), mais `gb_ll_audio_set_volume()` (verifie dans
`gb_ll_audio.c`) attend une echelle 0-255 ("0 = volume max, 116 =
volume min"), alors qu'`AkaRuntime::getMusicVolume()` renvoie 80 par
defaut sur une echelle qui semble pensee comme un pourcentage (0-100) --
passee telle quelle, le volume reel serait notablement plus faible que
prevu (~31% de l'echelle complete), meme si pas necessairement un
silence total. Mise a l'echelle ajoutee par precaution (x2.55) au
demarrage ET dans le callback de changement de volume. Signale
explicitement comme une HYPOTHESE, pas une certitude -- sans sortie
serie ESP32 pour verifier directement d'ici, impossible d'etre sur a
100% que c'est la cause complete.

Plusieurs erreurs d'edition de ma part (texte duplique, accolades
desequilibrees) trouvees et corrigees EN COURS de cette session --
reperees par controle systematique avant de continuer.

Tout revérifié après coup : syntaxe complète, AKA et SDL.

## Session 19 : musique OK confirmée ; bande de texte du bas + workflows CI

**Musique confirmée fonctionnelle** -- l'hypothese de mise a l'echelle
du volume (session 18) etait la bonne.

**Texte "UNDERGROUND LAIR" mal place.** Cause : position CENTREE
inventee par moi, qui ne correspond pas a l'original -- `darkUnderCOLOR.pde`
place ce texte a une position FIXE, alignee a gauche
(`text(description,50*multi,78*multi)`, jamais de centrage). Corrige :
x=50*2=100 fixe. Y decale de 12px vers le haut (mesure par Jicehel) --
Processing ancre le texte sur sa ligne de base par defaut (bas des
lettres), `drawText()` ici ancre sur le haut du glyphe, d'ou l'ecart.

**Texte residuel non efface.** Cause de fond, pas propre a un seul
ecran : aucun equivalent de `background(0)` (appele par l'original a
CHAQUE frame, sans exception) n'existait dans le portage. Sans lui,
une zone pas entierement recouverte de contenu opaque a chaque frame
(le texte du bas, dont la largeur change d'un message a l'autre) garde
les pixels de la frame precedente. Ajoute en tout premier dans
`GameApp::render()`.

**"DIALOGUE" -> "LOOTING".** Verifie dans le vrai code source :
l'original dit bien "DIALOGUE" (`Dialogue.pde`, `myDescription`) --
pas un bug de ma part, mais Jicehel prefere "LOOTING", plus clair.
Applique tel que demande.

**Bande du bas jamais mise a jour pendant le combat -- feature
entierement absente, pas un bug.** L'original affiche "A RAT ATTACKS"
puis alterne "YOU HIT RAT: 4DMG"/"RAT HITS: 4DMG" a chaque assaut, puis
"YOU KILLED RAT" (`Enemy.pde`). Particularite authentique reperee et
reproduite fidelement : le texte dit "4DMG" EN DUR des deux cotes,
quels que soient les PV reellement retires (`lastSentence="...: 4DMG"`
litteral dans le code source) -- confirme une incoherence deja notee
dans une session precedente sans avoir ete cablee. Systeme de messages
ajoute (`GameApp::combatMessage`, cles de traduction `CBT_*`).

**Bug trouve au passage (pas signale, repere en travaillant sur le
code) :** `lastSeenState` dans `updateCombat()` etait une variable
`static` LOCALE A LA FONCTION -- partagee entre TOUS les combats du
jeu au lieu d'etre propre a chacun. Transforme en membre de
`GameApp`, reinitialise a chaque nouvelle rencontre.

**Workflows GitHub Actions ajoutes** (`.github/workflows/`) :
- `build-pc.yml` : matrice Windows/Linux pour la build SDL de test
  (SDL2/SDL2_image/SDL2_mixer via apt sur Linux, vcpkg sur Windows).
- `build-aka.yml` : firmware AKA reel via l'action officielle
  `espressif/esp-idf-ci-action` -- un seul job (ESP-IDF cross-compile
  vers la meme cible quel que soit l'OS hote, pas besoin de matrice
  ici).

Les deux verifies comme YAML valide (pas encore executes reellement
sur GitHub -- a confirmer par Jicehel au premier push).

Tout revérifié après coup : syntaxe C++ complète, YAML valide.

## Session 20 : positions précises, inventaire en combat, sélecteur peu visible

**Positions -- 2e passe avec des mesures précises.** Bande du bas
("UNDERGROUND LAIR") décalée de 42px vers la gauche (7 caractères x
6px, police Narrow) et 2px vers le bas par rapport a la 1ere
correction (session 19), qui n'était pas encore la bonne. Le meme
biais touchant TOUS les messages (combat, victoire, butin, y compris
"Continuez") suggère une cause commune plutôt qu'un problème par écran
-- `kParchmentCenterX` de `Dialogue.cpp` décalé d'environ le même
montant (105 -> 65) par extrapolation, faute de mesure séparée précise
pour cet écran-là. Signalé comme une estimation, pas une certitude, à
confirmer.

**Inventaire inaccessible en combat -- vrai bug fonctionnel, pas un
problème visuel.** `actionC` (ouvrir l'inventaire) n'était vérifié que
dans `updateExploring()`, jamais dans `updateCombat()` -- impossible de
boire une potion de soin en plein combat, exactement comme rapporté.
Corrigé avec un vrai suivi de "où revenir" (`modeBeforeInventory`,
même principe que `modeBeforeDialogue` déjà en place) plutôt qu'un
retour fixe à l'exploration.

**Sélecteur d'objet peu visible sur le parchemin.** Mesuré directement :
`BUT_inventoryOver.png` est un jaune vif (255,222,0), qui contraste mal
contre le parchemin beige/tan (tons chauds similaires). Recoloré en
cyan vif (0,220,255) lors de la conversion -- `convert_assets.py`
accepte maintenant un remplacement de couleur par image
(`"recolor": [[r,g,b],[r,g,b]]` dans la config), même forme/alpha,
juste la teinte changée. Bug trouvé en l'implémentant : le premier
essai comparait des tuples Python a des listes JSON (jamais égales),
corrigé avant régénération.

Tout revérifié après coup : syntaxe complète, régénération des 71
assets sans erreur.

## Session 21 : workflow de release (binaires Windows/Linux téléchargeables)

Ajouté `.github/workflows/release.yml`, distinct de `build-pc.yml`
(celui-là valide juste que ça compile a chaque push, sans rien
publier) : se déclenche sur un tag de version (`v1.0.0` etc.) ou
manuellement, construit la build PC/SDL sur Windows ET Linux, empaquete
l'exécutable avec ses ressources d'exécution (`data/`, `lang/`,
`music/`) et, côté Windows, les DLL SDL2 nécessaires (liaison
dynamique -- sans elles l'exe ne démarre pas chez quelqu'un qui n'a pas
SDL2 installé), puis attache les deux zips a une Release GitHub créée
automatiquement (`softprops/action-gh-release`).

Vérifié comme YAML valide, pas encore déclenché réellement sur GitHub
(nécessite un tag poussé ou un déclenchement manuel côté Jicehel pour
confirmer que l'assemblage des DLL fonctionne bien en pratique).

## Session 22 : deux zones de texte indépendantes, pas un décalage global

Jicehel a clarifié l'architecture réelle : la bande du bas est une
ZONE DE TEXTE INDÉPENDANTE (~21 caractères), distincte du parchemin,
avec son propre centre -- le texte y est CENTRÉ, pas positionné à un
point fixe avec un décalage constant comme je l'avais implémenté.
Cause de l'erreur précédente : "UNDERGROUND LAIR" (16 caractères)
tombait par coïncidence presque juste avec un décalage fixe, ce qui
masquait le vrai problème -- tout autre message de longueur différente
aurait été mal placé.

**Corrigé** : centrage réel calculé par message
(`kBottomZoneCenterX - strlen(texte)*6/2`), centre de zone (96, unités
doublées) calibré directement à partir de la mesure précise de Jicehel
sur "UNDERGROUND LAIR" ("10px trop à droite, 1px trop haut" par rapport
à la position précédente) plutôt que deviné.

**Parchemin de dialogue -- sur-correction annulée.** La session
précédente avait appliqué la MÊME logique de décalage global au
parchemin, en supposant à tort une cause commune avec la bande du bas.
Jicehel confirme que cette zone était déjà correcte (centrage par
message déjà en place et fonctionnel), à part "Continuez"
spécifiquement -- `kParchmentCenterX` revenu à 105 (valeur d'avant la
sur-correction). Pas de nouvelle tentative de deviner l'ajustement fin
pour "Continuez" seul -- en attente d'une mesure aussi précise que
celle donnée pour la bande du bas avant de retoucher.

Tout revérifié après coup : syntaxe complète.

## Session 23 : ajustement ciblé pour "Continuez"

Jicehel confirme le principe déjà en place (centrage réel par nombre
de caractères x largeur de police, pas une position fixe) et donne une
estimation "à la louche" de ~20px pour le décalage restant de
"Continuez" spécifiquement.

Vérifié : le mécanisme de centrage (`centeredX()`, même police Wide,
même formule) ne présente pas d'incohérence de code identifiable par
rapport aux autres textes de l'écran de victoire (qui utilisent
exactement le même appel) -- pas d'explication solide trouvée pour un
décalage propre à cette seule chaîne. Ajustement isolé de 20px vers la
gauche appliqué directement a ce texte specifique (pas au calcul
général, qui semble correct pour le reste de l'écran) plutôt que
d'inventer une théorie non vérifiée.

Vérifié après coup : syntaxe complète.

## Session 24 : cache CMake périmé committé par erreur (build-aka.yml échouait)

Jicehel a fourni le log d'échec ET un diagnostic déjà rédigé (par un
tiers) -- vérifié contre le log avant d'appliquer : diagnostic correct.
Un dossier `build/` généré localement sur Windows (`CMakeCache.txt`
fige en dur le chemin absolu source/binaire de sa génération, ici
`C:/Users/jean_/OneDrive/.../dark_and_under_color_aka`) avait été
commis dans le dépôt -- le runner Linux de GitHub Actions, avec un
chemin de projet totalement différent, refuse de le réutiliser
("Build directory ... configured for project ... not ...").

**Corrigé dans les TROIS workflows** (pas seulement `build-aka.yml` --
`build-pc.yml` et `release.yml` utilisent aussi CMake et auraient le
même problème si un `build/` local y était un jour commis par erreur) :
`rm -rf build` avant chaque configuration, plutôt que de compter sur
`idf.py fullclean` (le cache pointe vers un chemin qui n'existe même
pas sur le runner -- fullclean ne suffit pas forcément).

**`.gitignore` ajouté** (`build/`, `sdkconfig.old`) pour empêcher que
ça se reproduise. Si `build/` est déjà suivi dans le dépôt de Jicehel,
il devra le retirer une fois avec `git rm -r --cached build` avant son
prochain commit -- le `.gitignore` seul n'affecte que les fichiers pas
encore suivis.

Vérifié : YAML valide sur les trois workflows, aucun `build/` présent
dans cette copie de travail (confirmant que le problème venait bien du
dépôt local de Jicehel, pas d'un oubli de ma part).

## Session 25 : VCPKG_ROOT vide sur le runner Windows (build-pc.yml)

Même schéma que la session précédente : diagnostic + correctif déjà
rédigés fournis par Jicehel, vérifiés contre le log avant application.
Le message d'erreur (`/scripts/buildsystems/vcpkg.cmake`, rien avant le
"/") confirme exactement le diagnostic : `$env:VCPKG_ROOT` était vide
au moment de construire ce chemin -- pas garanti défini sur tous les
runners `windows-latest` malgré vcpkg lui-même bien présent.

**Corrigé dans `build-pc.yml` ET `release.yml`** (même construction de
chemin dans les deux) : résolution de l'emplacement réel de vcpkg via
`Get-Command vcpkg` (retrouve l'exécutable effectivement sur le PATH)
plutôt que de dépendre d'une variable d'environnement. `release.yml`
simplifié au passage : l'étape Configure reconstruisait le même chemin
une seconde fois au lieu de réutiliser `CMAKE_TOOLCHAIN_FILE` déjà posé
par l'étape d'installation (comme le fait déjà `build-pc.yml`).

Vérifié : YAML valide sur les deux fichiers modifiés.

## Session 26 : collision Time.h/time.h sur Windows + release.yml restructuré

**Build Windows échouait sur `SdlTranslator.cpp`
("'clock_t' is not a member of global namespace").** Diagnostic fourni
par Jicehel, vérifié en cherchant le fichier réel avant d'appliquer --
confirmé : `shared/platform/Time.h` existait bien. Sur Linux/macOS
(sensibles à la casse), aucun souci ; sur Windows/NTFS (insensible à la
casse), `Time.h` et `time.h` sont LE MÊME FICHIER -- ce header
personnel écrasait le vrai `<time.h>` du CRT MSVC dès qu'un header
standard (`<ctime>`, inclus via `SdlTranslator.cpp`) tentait de
l'inclure. Renommé en `PlatformTime.h`, les deux fichiers qui
l'incluaient (`AkaInput.cpp`, `SdlInput.cpp`) mis à jour.

**`release.yml` échouait sur un déclenchement manuel** ("GitHub
Releases requires a tag") -- `workflow_dispatch` sans tag ne fournit
rien à `github.ref_name`. Ajouté un input `release_tag` obligatoire
pour les déclenchements manuels, utilisé en repli
(`inputs.release_tag || github.ref_name`).

**Amélioration de fiabilité appliquée au passage** (signalée par
Jicehel, pas juste le correctif minimal) : les deux jobs de la matrice
(Windows/Linux) tentaient chacun de créer/modifier la MÊME Release
GitHub en même temps -- risque de course. Restructuré : chaque job de
build dépose son zip en artefact de workflow, un job `release` séparé
(`needs: build`) télécharge les deux artefacts et crée la Release une
seule fois.

Vérifié : YAML valide, les deux fichiers C++ renommés compilent
toujours, aucun CMakeLists ne référençait l'ancien nom directement.

## Session 27 : renommage Time.h incomplet -- Enemy.cpp oublié

Retour de build (Linux ET Windows, même erreur) : `Enemy.cpp:3:10:
fatal error: ../../platform/Time.h: No such file or directory`. Le
renommage de `Time.h` en `PlatformTime.h` (session 26) était incomplet
-- ma recherche de l'époque (`grep '#include "Time.h"'`) ne trouvait
que la forme d'inclusion simple, pas la forme à chemin relatif
utilisée par `Enemy.cpp` (`#include "../../platform/Time.h"`), donc ce
fichier est passé au travers. Corrigé, et recherche refaite en plus
large cette fois (`Time\.h` sans ancrage sur la syntaxe d'inclusion)
pour confirmer qu'aucune autre référence ne traîne.

Leçon retenue pour la suite : après ce genre de renommage, vérifier
CHAQUE fichier individuellement avec les MÊMES chemins d'inclusion que
le vrai `CMakeLists.txt`, pas seulement les fichiers trouvés par une
recherche de texte qui peut elle-même rater des variantes de syntaxe --
fait cette fois pour l'ensemble du projet (SDL et AKA), pas seulement
les fichiers touchés par la correction.

## Session 28 : échec de lien Windows -- SDL2_image.lib introuvable

Diagnostic vérifié contre le vrai `CMakeLists.txt` avant application :
confirmé. `target_link_libraries` utilisait les noms bruts (`SDL2_image`,
`SDL2_mixer`) au lieu des cibles CMake exportées par vcpkg. Ça
fonctionnait sur Linux par coïncidence -- CMake, ne reconnaissant pas
ces noms comme des cibles, retombe sur la convention `-lSDL2_image` du
lieur Linux, qui trouve la bibliothèque via les chemins système
standards. Windows/MSVC n'a pas cette convention : il lui faut soit un
`.lib` exact dans son chemin de recherche, soit la vraie cible CMake
qui sait précisément où vcpkg l'a installé.

**Corrigé avec prudence, pas juste le correctif proposé tel quel** :
forcer `find_package(... CONFIG REQUIRED)` partout risquait de casser
Linux si le paquet `apt` de la plateforme utilisée par `ubuntu-latest`
ne fournit pas de fichier de config CMake (pas garanti selon la
version) -- CONFIG est strict, pas de repli automatique vers le mode
Module comme le fait `find_package` normal. Le `CMakeLists.txt` détecte
maintenant si les cibles modernes existent (`SDL2::SDL2`,
`SDL2_image::SDL2_image`, etc. -- garanti côté vcpkg/Windows) et les
utilise si oui, sinon retombe sur l'ancienne méthode par variables (qui
fonctionnait déjà sous Linux) -- aucun risque de régression là où ça
marchait déjà. `VCPKG_TARGET_TRIPLET=x64-windows` ajouté à l'étape
Configure des deux workflows concernés (`build-pc.yml`, `release.yml`)
pour la cohérence de triplet suggérée.

Pas de `cmake` disponible dans ce bac à sable pour tester réellement la
configuration -- relu attentivement la syntaxe (if/else/endif
équilibrés), mais le premier vrai test reste côté Jicehel.

## Session 29 : écran noir côté PC (SDL) -- la vraie cause, pas un souci de chemin

Jicehel rapporte : musique OK, écran noir. Analyse en deux temps.

**Vraie cause trouvee (majeure)** : `platform_sdl/Assets.cpp` (la table
qui associe chaque `ImageId` a son nom de fichier PNG pour le
chargement a chaud) n'avait JAMAIS ete tenue a jour en parallele de son
equivalent AKA -- seulement les 12 toutes premieres entrees (les
objets, ajoutees tout au debut du portage), alors que 71 assets
existent desormais (`UIMain`, tous les murs du couloir, les ecrans
Splash...). Chaque ajout d'asset au fil des ~20 sessions suivantes
n'a mis a jour que le cote AKA (`tools/all_assets_config.json` ->
`GeneratedAssets.cpp`), jamais le cote SDL en parallele -- derive
totale non detectee jusqu'ici, faute d'avoir teste la build SDL en
conditions reelles avant cette session.

**Corrige a la racine, pas juste rattrape une fois** : `convert_assets.py`
genere maintenant `platform_sdl/Assets.cpp` EN MEME TEMPS que
`GeneratedAssets.cpp`, depuis la MEME configuration -- les deux
fichiers ne peuvent plus deriver l'un de l'autre a l'avenir, un seul
endroit a maintenir. Bug trouve en l'implementant : mauvais calcul de
chemin de sortie (base sur le chemin de sortie AKA fourni en argument
plutot que sur l'emplacement du script lui-meme) -- corrige avant
regeneration.

**Amelioration de diagnostic ajoutee en plus** (pour toute cause
similaire a l'avenir) : les chemins `data/`/`lang/`/`music/` sont
maintenant resolus par rapport a l'emplacement REEL de l'executable
(`GetModuleFileNameA` sous Windows, `/proc/self/exe` sous Linux) plutot
que par rapport au dossier de travail courant, qui ne correspond pas
toujours au dossier de l'executable selon comment il est lance.
`SdlRenderer::verifyAssetsLoadable()` verifie au demarrage qu'un asset
connu se charge, et affiche une VRAIE fenetre d'erreur visible
(`SDL_ShowSimpleMessageBox`) sinon -- l'ancien message d'erreur partait
dans `stderr`, invisible sur un exe lance en double-clic (pas de
console attachee).

Verifie : les 71 entrees sont bien presentes des deux cotes apres
regeneration (confirme par recherche directe de `UIMain`/`VisionBack`
dans le fichier SDL genere), compilation complete des fichiers
modifies.

## Session 30 : zoom PC configurable (x2 par défaut, x3/x4 au choix)

Demandé par Jicehel. Implémenté sans toucher à AUCUN appel de dessin
existant : tout le code de jeu (`Vision`, `Dialogue`, `UI`...) suppose
un espace de rendu fixe de 300x162 ("logique x2") -- changer ce
facteur en dur dans des dizaines d'endroits aurait été risqué pour peu
de gain. Utilisé `SDL_RenderSetLogicalSize(renderer, 300, 162)` à la
place : l'espace de rendu interne reste STRICTEMENT fixe, SDL met à
l'échelle automatiquement vers la taille RÉELLE de la fenêtre (elle,
configurable) -- zéro risque de régression sur le rendu existant, texte
compris (le mécanisme SDL s'applique uniformément à toutes les
primitives, pas seulement aux textures).

Zoom choisi via un argument en ligne de commande au lancement (2, 3 ou
4 -- ex. `darkandundercolor_pc.exe 3`), x2 par défaut si omis ou si
la valeur donnée est hors de cette plage (avec un message d'avertissement
dans ce dernier cas plutôt qu'un plantage).

Vérifié : compilation complète des deux fichiers modifiés.

## Session 31 : menu de zoom cliquable en haut de fenêtre (PC)

Demandé par Jicehel -- une vraie barre "ZOOM: x2 x3 x4" cliquable à la
souris, plutôt que l'argument en ligne de commande de la session
précédente.

**Souris ajoutée à `SdlInput`** (n'existait pas du tout jusqu'ici --
le jeu n'utilise que le clavier depuis le passage aux boutons
physiques) : position + clic-ce-tour, PC uniquement, pas dans
`IInput` (AKA n'a pas de souris, rien à y faire).

**`SDL_RenderSetLogicalSize` (session précédente) abandonné** : son
cadrage automatique s'applique à toute la fenêtre, incompatible avec
une barre de menu en haut. Remplacé par un contrôle manuel
(`beginGameArea()` : viewport décalé sous la barre + échelle
`zoom/2`, le jeu continuant de dessiner exactement comme avant en
coordonnées "300x162" ; `renderMenuBar()` : viewport plein écran,
échelle 1:1, coordonnées réelles non affectées par le zoom -- texte du
menu toujours la même taille quel que soit le zoom choisi).

**Changement d'architecture partagée, sans impact sur AKA** :
`GameApp::render()` appelait `renderer.present()` en interne --
impossible d'insérer le dessin de la barre entre le jeu et
l'affichage final sans décalage d'une frame. Déplacé à la charge de
l'appelant (chaque `main.cpp`) ; le `main.cpp` AKA appelle
maintenant `present()` explicitement juste après `render()`, au même
endroit qu'avant -- comportement strictement identique côté AKA.

Zoom sélectionné en cliquant sur x2/x3/x4 redimensionne la fenêtre en
direct (`SDL_SetWindowSize`), pas besoin de relancer l'exécutable.

Vérifié : compilation complète des fichiers modifiés (SDL et AKA).

## Session 32 : panneau d'aide dans la barre de menu PC

Demandé par Jicehel -- garder le menu système AKA tel quel (propre à
la console) et ajouter, côté PC uniquement, un panneau d'aide dans la
barre déjà en place.

Bouton "?" ajouté à côté des boutons de zoom. Au clic, ouvre un
panneau qui couvre le reste de la fenêtre (fond semi-transparent
par-dessus le jeu) avec :
- **Commandes** : correspondance touche PC -> bouton AKA -> à quoi ça
  sert (Haut/Bas, Gauche/Droite, Q/E=L1/R1, Z=A, X=B, C, V).
- **Autres versions** : console Gamebuino AKA vs cette build PC de test.
- **Crédits** : repris du `LICENSE` pour rester cohérent (Cyril
  Guichard/Garage Collective, Press Play On Tape, musique Visager
  CC BY 4.0, police Simple 5x8 Atom596 FFC, portage Jicehel).
- **Lien GitHub** du dépôt.

Au passage, mis à jour les crédits du menu système AKA lui-même
(`akaRuntime.setCredits()`), restés à un texte provisoire ("licence à
confirmer") depuis une session bien antérieure alors que le vrai
`LICENSE` existe maintenant -- même contenu que le panneau PC, pour
rester cohérent entre les deux versions.

Vérifié : compilation complète (PC et AKA).

## Session 33 : contrôle total -- en-têtes, obsolescence, documentation GitHub

Demandé par Jicehel pendant qu'il teste de son côté : audit complet du
projet (51 fichiers source), pas de correction de bug cette fois.

**En-têtes de fichiers** : 6 fichiers avec un en-tête absent ou réduit
à une ligne complétés (`AkaInput.h/.cpp`, `AkaTranslator.h`,
`SdlInput.cpp`, `Splash.cpp`) -- plus `SdlInput.h`, dont l'en-tête
existant était devenu FAUX (disait encore "pas encore de vraie
configuration de touches", alors que souris + menu de zoom existent
depuis), corrigé plutôt que juste complété.

**Commentaires obsolètes trouvés et corrigés** : `Level00.cpp`
affirmait encore que le rôle de `TileType::Special` n'était "pas
encore défini côté original" -- alors que ça a été confirmé (variante
décorative de mur, pas une porte) en portant `Vision.pde`, il y a
longtemps. `Player.h` reformulé légèrement (design toujours valide,
juste la formulation datait d'avant que tout soit porté).

**Code et fichiers morts retirés** : `SplashLargeButton`/
`SplashLargeButtonOver` (2 assets jamais utilisés depuis que les
boutons tactiles PLAY/CREDITS ont été remplacés par du texte, ~9 Ko de
flash gaspillés) supprimés de `AssetIds.h` et de la config -- 69 assets
au lieu de 71 désormais. Trois fichiers de config intermédiaires
(`objects_config.json`, `world_config.json`, `ui_config.json`),
fusionnés depuis longtemps dans `all_assets_config.json` mais jamais
supprimés, retirés -- plus rien ne les référençait.

**Documentation GitHub** : `README.md` créé (n'existait pas du tout --
seul `README_STATUS.md`, un journal de session par session, existait,
pas une vraie page d'accueil). Couvre présentation, état actuel,
compilation (AKA + PC + CI/releases), commandes, structure du dépôt,
langues, licence. `README_STATUS.md` recadré comme journal détaillé
plutôt que doc principale, avec un renvoi clair vers `README.md` en
tête -- son propre paragraphe d'ouverture était lui-même perimé
("licence à confirmer", "jamais testé pour de vrai") depuis une
trentaine de sessions, corrigé.

Tout revérifié après coup : compilation complète, JSON valide.

### Améliorations proposées (pas appliquées, à la discrétion de Jicehel)

- **Langues** : seuls fr/en traduits, de/es/it supportés par le système
  mais vides -- à compléter si besoin.
- **"Continuez" mal centré** (session 23) : corrigé par un ajustement
  isolé de -20px sans cause identifiée avec certitude dans le code --
  fonctionne, mais reste une rustine plutôt qu'une correction de fond.
  À revisiter si un jour la cause réelle se manifeste ailleurs.
- **Pas de bruitages (SFX)**, seulement la musique de fond -- absent de
  cette passe, jamais demandé jusqu'ici.
- **Un seul niveau fourni** ("Underground Lair") -- en ajouter d'autres
  suivrait exactement le patron de `Level00.cpp`.
- **`README_STATUS.md` a atteint ~1100 lignes** -- toujours utile comme
  historique detaille, mais pourrait valoir une restructuration en
  format CHANGELOG (le plus recent en premier, ou resume par grandes
  etapes) si sa taille devient genante a naviguer.

## Session 34 : traduction allemand/espagnol/italien

Les 3 langues manquantes (le système en gère 5, seuls fr/en étaient
traduits jusqu'ici) complétées -- 39 clés chacune, correspondance
exacte avec les clés françaises vérifiée programmatiquement (aucune
manquante, aucune en trop).

**Sans caractères accentués, volontairement** : ni la police système
8x8 (`font8x8_basic`) ni la police 5x8 "Simple 5x8" ne couvrent autre
chose que l'ASCII imprimable (32-126) -- vérifié avant de traduire,
pas après coup. Allemand sans umlauts (ue/oe/ae/ss), espagnol et
italien sans accents ni ñ -- pratique courante pour ce genre de
contrainte de police sur les jeux pixel-art.

**Longueurs vérifiées précisément** (pas à l'oeil) contre les deux
limites déjà établies : ~16 caractères pour les textes affichés sur le
parchemin, ~28 pour l'écran Commandes du menu système -- aucune des
3 langues ne dépasse ces limites sur aucune clé.

**Particularité de l'original reproduite dans les 3 langues** : le
"4DMG" affiché en dur (quels que soient les PV réellement retirés,
voir session 19) traduit en abréviation locale plutôt que laissé en
anglais -- "4SCH" (Schaden) en allemand, "4DA" (daño) en espagnol,
"4DN" (danno) en italien -- même principe que "4DGT" déjà utilisé en
français.

Fichiers écrits dans `sdcard_files/DarkUnderColor/lang/` (AKA) et
copiés vers `platform_sdl/lang/` (test PC), comme pour fr/en. JSON
valide sur les 3.

## Session 35 : correction du "4DMG" ambigu -- vraies valeurs affichées

Demandé par Jicehel : le "4DMG" fixe (reproduit fidèlement depuis
l'original, signalé comme "voulu" dans une session précédente) était
trop ambigu pour être gardé tel quel.

**Corrigé** : les messages de combat affichent maintenant les vraies
valeurs (1 PV pour l'ennemi vers le joueur, 2 PV pour le joueur vers
l'ennemi -- `%s HITS: 1DMG` / `YOU HIT %s: 2DMG`), dans les 5 langues.

**Pour ne pas recréer le même genre de bug plus tard** : les deux
valeurs de dégâts, jusqu'ici deux "2" et "1" indépendants dans
`Enemy.cpp` et `GameApp.cpp` (exactement le genre de duplication qui
avait causé le "4DMG" figé de l'original), centralisées dans
`Config.h` (`kEnemyDamageToPlayer`, `kPlayerDamageToEnemy`) -- un futur
changement d'équilibrage ne peut plus faire diverger le texte affiché
de l'effet réel, puisque les deux lisent la même constante.

Commentaires mis à jour en conséquence dans `Enemy.h` (le "BUG REPÉRÉ,
PAS CORRIGÉ" documenté en session 19 n'est plus d'actualité).

Vérifié : compilation complète, JSON valide sur les 10 fichiers de
langue touchés (5 langues x 2 emplacements).
