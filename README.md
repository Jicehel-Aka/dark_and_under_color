# Dark & Under (couleur) — portage Gamebuino AKA

Portage du remake couleur (Processing/Java) de **Dark & Under**, jeu
d'exploration de donjon en vue subjective, vers la console
**Gamebuino AKA** (ESP32-S3) — et vers PC (Windows/Linux) pour tester
sans matériel.

- Concept original, code, art & design : **Cyril Guichard (Garage
  Collective)**
- Concept original Dark & Under (Arduboy) : **Press Play On Tape**
- Portage AKA/PC : **Jicehel**

## État actuel

Le jeu tourne de bout en bout sur les deux cibles : écran-titre,
exploration du donjon en pseudo-3D, combat, inventaire, mini-carte,
dialogues, musique de fond. Un seul niveau fourni pour l'instant
("Underground Lair").

Pour le détail complet de chaque étape du portage (bugs trouvés et
corrigés, décisions d'architecture, historique complet) : voir
[`README_STATUS.md`](README_STATUS.md).

## Compiler

### Firmware AKA (ESP-IDF)

Nécessite [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) 5.5.1
ciblant `esp32s3`.

```
idf.py set-target esp32s3
idf.py build
idf.py -p <PORT> flash
```

Copier ensuite le contenu de `sdcard_files/DarkUnderColor/` sur la
carte SD de la console (dossier `DarkUnderColor/` à la racine de la
carte, à côté des autres jeux).

### Build PC de test (SDL2)

Nécessite SDL2, SDL2_image et SDL2_mixer.

```
cmake -S platform_sdl -B build
cmake --build build
```

L'exécutable a besoin du dossier `music/` de `platform_sdl/` à côté de
lui pour fonctionner (seule chose encore externe -- images et langues
sont directement embarquées dans le binaire, aucun autre dossier à
copier ; déjà le cas si vous lancez depuis le dossier de build généré
par CMake).

Zoom réglable en jeu via un petit menu en haut de la fenêtre (x2 par
défaut, x3/x4 au choix), ou en argument de ligne de commande
(`darkandundercolor_pc 3` pour x3).

### CI / Releases GitHub Actions

Trois workflows dans [`.github/workflows/`](.github/workflows/) :

| Workflow | Déclencheur | Rôle |
|---|---|---|
| `build-aka.yml` | push/PR touchant le firmware | Vérifie que le firmware AKA compile |
| `build-pc.yml` | push/PR touchant la build PC | Vérifie que la build PC compile sur Windows **et** Linux |
| `release.yml` | tag `v*.*.*` poussé, ou déclenchement manuel | Compile et publie des binaires PC téléchargeables (Windows + Linux) sous forme de [Release GitHub](../../releases) |

Pour publier une nouvelle release : pousser un tag (`git tag v1.0.0 &&
git push --tags`) ou lancer `release.yml` manuellement depuis l'onglet
Actions (un tag doit alors être saisi à la main).

## Commandes

| Touche PC | Bouton AKA | Action |
|---|---|---|
| Haut / Bas (ou W / S) | D-pad haut/bas | Avancer / reculer |
| Gauche / Droite (ou A / D) | D-pad gauche/droite | Tourner le regard |
| Q / E | L1 / R1 | Déplacement latéral |
| Z / Espace | A | Valider / attaquer / utiliser |
| X / Retour arrière | B | Retour / refuser / jeter |
| C / Suppr | C | Inventaire |
| V / Entrée | D | Mini-carte |

Les touches PC sont données par leur **position physique** (mêmes
touches quel que soit AZERTY/QWERTY/QWERTZ) : W/A/S/D + Q/E forment le
schéma FPS classique, Z/X/C/V la rangée d'actions juste en dessous.

Sur AKA, le bouton MENU ouvre le menu système (volume, retour au
loader, langue, crédits) ; un appui long dessus prend une capture
d'écran.

Côté PC, un panneau d'aide (bouton **?** dans la barre en haut de la
fenêtre) reprend ce tableau, plus les informations de version et de
crédits.

## Structure du dépôt

```
main/                  Point d'entrée AKA (composant ESP-IDF "main")
components/
  platform_aka/         Rendu/entrée/traduction AKA + logique de jeu (compilée pour AKA)
  gamebuino/             Bibliothèque AKA (gb_core, gb_graphics, gb_audio_player...)
  aka_runtime/            Coquille système AKA (menu, langue, sauvegarde...)
platform_sdl/           Build PC de test (SDL2) — rendu/entrée/traduction PC
shared/
  game/                  Logique de jeu commune aux deux plateformes (entités, écrans, niveaux)
  platform/               Interfaces communes (IRenderer, IInput, ITranslator...)
sdcard_files/            Fichiers à copier sur la carte SD de la console (langues, musique)
tools/                   Script de conversion des assets (PNG -> RGB565, config unique pour AKA et PC)
.github/workflows/       CI et publication de releases
```

## Langues

Français et anglais disponibles (`sdcard_files/DarkUnderColor/lang/`).
Allemand/espagnol/italien pas encore traduits (le système en gère
jusqu'à 5).

## Licence

Voir [`LICENSE`](LICENSE) — code sous licence BSD 3-Clause, les
ressources graphiques restent la propriété exclusive de leur auteur
(licence distincte). Musique et police tierces créditées séparément
dans le même fichier.
