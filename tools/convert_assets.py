#!/usr/bin/env python3
"""
convert_assets.py — Convertit les PNG du projet couleur en tableaux
C uint16_t RGB565 exploitables par gb_graphics::drawImage/drawImageScaled
(voir platform_aka/AkaRenderer.h).

Transparence : gb_graphics utilise une couleur-clé simple (pas de canal
alpha reel -- confirme en lisant gb_graphics_image.cpp), donc tout pixel
dont l'alpha PNG est en dessous d'un seuil est remplace par une couleur
"magique" improbable dans un vrai dessin (magenta pur, 0xF81F en RGB565),
et cette meme valeur doit etre passee en parametre transparent_color a
l'appel de drawImage cote jeu.

Usage :
  python3 convert_assets.py <config.json> <dossier_png_source> <fichier_cpp_sortie>

config.json : liste de {"id": "NomImageAsset", "file": "fichier.png"}
"""
import sys, json, os
from PIL import Image, ImageEnhance

TRANSPARENT_KEY = 0xF81F  # magenta pur (R=31,G=0,B=31) -- symetrique R/B, donc valide dans les deux ordres de bits

def rgb565(r, g, b):
    # BUG TROUVE ET CORRIGE : le vrai gb_graphics (lcd_color_rgb, voir
    # gb_ll_lcd.h) range ROUGE dans les bits BAS et BLEU dans les bits
    # HAUTS -- l'inverse du RGB565 "standard" (R haut, B bas), malgre le
    # nom de la fonction. Verifie directement dans la formule reelle :
    # "(red>>3) | ((green>>2)<<5) | ((blue>>3)<<11)". Toutes les images
    # converties avec l'ancienne formule avaient donc rouge et bleu
    # intervertis (le logo Garage Collective orange apparaissait bleu).
    #
    # BUG TROUVE ET CORRIGE (2e passe, ecrans Splash/Credits trop
    # sombres signales par Jicehel apres test reel) : ">>3"/">>2"
    # TRONQUENT la partie fractionnaire au lieu d'arrondir au plus
    # proche -- biais systematique vers le bas sur TOUTE conversion 8
    # bits -> 5/6 bits, pas seulement Splash/Credits (juste plus visible
    # sur ces grands aplats de couleur uniforme). Corrige en ajoutant un
    # demi-pas avant de tronquer (arrondi standard), remonte legerement
    # la luminosite de toutes les images converties.
    r5 = ( r + 4 ) >> 3
    g6 = ( g + 2 ) >> 2
    b5 = ( b + 4 ) >> 3
    if r5 > 31: r5 = 31
    if g6 > 63: g6 = 63
    if b5 > 31: b5 = 31
    return ( b5 << 11 ) | ( g6 << 5 ) | r5

def convert_image(path, gamma=None, recolor=None):
    im = Image.open(path).convert("RGBA")
    if recolor:
        # Remplace une couleur precise par une autre (meme forme/alpha,
        # juste la teinte) -- utilise pour InventorySelectHighlight :
        # jaune vif (255,222,0) sur fond de parchemin beige/tan a un
        # contraste tres faible (signale par Jicehel, "se voit peu").
        px = im.load()
        old, new = recolor
        for y in range(im.height):
            for x in range(im.width):
                r, g, b, a = px[x, y]
                if (r, g, b) == old:
                    px[x, y] = (new[0], new[1], new[2], a)
    if gamma is not None:
        # BUG TROUVE ET CORRIGE (2e passe) : gamma=1.8 choisi "a l'oeil"
        # sans reference precise -- SURCORRIGE largement une fois compare
        # aux vraies captures d'ecran du jeu en fonctionnement fournies
        # par Jicehel. Mesure directe : source brute (avant tout
        # rehaussement) deja tres proche de la cible reelle -- gamma
        # calcule a partir de deux points de mesure (source vs capture
        # de reference) : 0.901 (credits) et 0.958 (titre), moyenne
        # retenue 0.93 (leger assombrissement, pas un eclaircissement).
        alpha = im.split()[3]
        rgb = im.convert("RGB")
        inv_gamma = 1.0 / gamma
        lut = [int((i / 255.0) ** inv_gamma * 255) for i in range(256)]
        rgb = rgb.point(lut * 3)
        rgb = ImageEnhance.Contrast(rgb).enhance(1.10)
        im = rgb.convert("RGBA")
        im.putalpha(alpha)
    w, h = im.size
    pixels = list(im.getdata())
    out = []
    for (r, g, b, a) in pixels:
        if a < 128:
            out.append(TRANSPARENT_KEY)
        else:
            v = rgb565(r, g, b)
            # Evite qu'un pixel opaque tombe pile sur la couleur-cle par
            # hasard (deviendrait transparent par erreur) -- decale d'1
            # bit de bleu, imperceptible visuellement.
            if v == TRANSPARENT_KEY:
                v ^= 0x0001
            out.append(v)
    return w, h, out

def main():
    if len(sys.argv) != 4:
        print(__doc__)
        sys.exit(1)

    config_path, src_dir, out_path = sys.argv[1], sys.argv[2], sys.argv[3]
    with open(config_path, encoding="utf-8") as f:
        entries = json.load(f)

    lines = []
    lines.append("// GeneratedAssets.cpp — GENERE AUTOMATIQUEMENT par tools/convert_assets.py")
    lines.append("// Ne pas editer a la main -- relancer le script si les PNG source changent.")
    lines.append('#include "AkaRenderer.h"')
    lines.append('#include "AssetIds.h"')
    lines.append("")
    lines.append(f"constexpr uint16_t kTransparentColorKey = 0x{TRANSPARENT_KEY:04X};")
    lines.append("")

    table_entries = []
    total_bytes = 0
    for entry in entries:
        image_id = entry["id"]
        file_name = entry["file"]
        path = f"{src_dir}/{file_name}"
        recolor = ( tuple(entry["recolor"][0]), tuple(entry["recolor"][1]) ) if "recolor" in entry else None
        w, h, pixels = convert_image(path, gamma=entry.get("gamma"), recolor=recolor)
        total_bytes += len(pixels) * 2
        array_name = f"k{image_id}Pixels"
        lines.append(f"// {file_name} ({w}x{h})")
        lines.append(f"static const uint16_t {array_name}[{len(pixels)}] = {{")
        row_items = []
        for i, v in enumerate(pixels):
            row_items.append(f"0x{v:04X}")
            if len(row_items) == 16:
                lines.append("    " + ", ".join(row_items) + ",")
                row_items = []
        if row_items:
            lines.append("    " + ", ".join(row_items) + ",")
        lines.append("};")
        lines.append(f"static const AkaAsset k{image_id}Asset = {{ {array_name}, {w}, {h} }};")
        lines.append("")
        table_entries.append((image_id, f"k{image_id}Asset"))

    lines.append("const AkaAsset* getAkaAsset( ImageId id ) {")
    lines.append("    switch ( static_cast<ImageAsset>( id ) ) {")
    for image_id, asset_var in table_entries:
        lines.append(f"        case ImageAsset::{image_id}: return &{asset_var};")
    lines.append("        default: return nullptr;")
    lines.append("    }")
    lines.append("}")

    with open(out_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")

    print(f"OK : {len(entries)} images converties, {total_bytes} octets ({total_bytes/1024:.1f} Ko) dans {out_path}")

    # BUG TROUVE ET CORRIGE (ecran noir cote SDL, musique OK -- signale
    # par Jicehel) : platform_sdl/Assets.cpp (la table ImageId -> nom de
    # fichier PNG pour le chargement a chaud) n'avait jamais ete tenue a
    # jour en parallele de celle-ci -- seulement les 12 toutes premieres
    # entrees (les objets, du tout debut du portage), alors que 71
    # assets existent desormais (UIMain, les murs, les ecrans Splash...).
    # Generee desormais depuis la MEME config a chaque conversion,
    # plutot que maintenue a la main en double -- ne peut plus deriver.
    sdl_lines = []
    sdl_lines.append("// Assets.cpp — GENERE AUTOMATIQUEMENT par tools/convert_assets.py")
    sdl_lines.append("// (meme config que components/platform_aka/GeneratedAssets.cpp --")
    sdl_lines.append("// ne plus editer a la main, les deux fichiers derivaient l'un de")
    sdl_lines.append("// l'autre auparavant, cause d'un ecran noir cote SDL).")
    sdl_lines.append('#include "Assets.h"')
    sdl_lines.append('#include "AssetIds.h"')
    sdl_lines.append("")
    sdl_lines.append("const char* assetFileName( ImageId id ) {")
    sdl_lines.append("    switch ( static_cast<ImageAsset>( id ) ) {")
    for entry in entries:
        sdl_lines.append(f'        case ImageAsset::{entry["id"]}: return "{entry["file"]}";')
    sdl_lines.append("        default: return nullptr;")
    sdl_lines.append("    }")
    sdl_lines.append("}")

    sdl_out_path = os.path.join( os.path.dirname( os.path.abspath( __file__ ) ), "..", "platform_sdl", "Assets.cpp" )
    sdl_out_path = os.path.normpath( sdl_out_path )
    if not os.path.isdir( os.path.dirname( sdl_out_path ) ):
        sdl_out_path = None
    if sdl_out_path:
        with open( sdl_out_path, "w", encoding="utf-8" ) as f:
            f.write( "\n".join( sdl_lines ) + "\n" )
        print(f"OK : table SDL regeneree en parallele dans {sdl_out_path}")

if __name__ == "__main__":
    main()
