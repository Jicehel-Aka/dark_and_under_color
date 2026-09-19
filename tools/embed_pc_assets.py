#!/usr/bin/env python3
"""
embed_pc_assets.py — Embarque tous les fichiers d'execution de la build
PC (data/*.png, lang/*.json, music/*.wav) directement dans l'executable,
demande par Jicehel ("mettre tous les fichiers necessaires dans
l'executable") plutot que de devoir copier des dossiers a cote de l'exe.

Genere un .cpp par categorie (images/lang/musique) avec les octets bruts
en tableaux C, plus une petite table de correspondance nom -> {donnees,
taille}. SdlRenderer/SdlTranslator/main.cpp chargent ensuite via
SDL_RWFromConstMem au lieu d'ouvrir des fichiers.

Prevenu : le fichier musique (~9,5 Mo) genere un GROS fichier source une
fois converti en tableau C (les compilateurs digerent ca, mais plus
lentement qu'un petit fichier) -- compromis assume pour repondre
litteralement a la demande d'un executable autonome.
"""
import sys, os

def c_identifier(name):
    return name.replace(".", "_").replace("-", "_")

def emit_bytes_array(f, var_name, data):
    f.write(f"static const unsigned char {var_name}[] = {{\n")
    for i in range(0, len(data), 20):
        chunk = data[i:i+20]
        f.write("    " + ",".join(str(b) for b in chunk) + ",\n")
    f.write("};\n")
    f.write(f"static const unsigned int {var_name}_len = {len(data)};\n\n")

def process_dir(src_dir, out_cpp, table_name, entry_type):
    files = sorted(os.listdir(src_dir))
    with open(out_cpp, "w", encoding="utf-8") as f:
        f.write(f"// {os.path.basename(out_cpp)} — GENERE AUTOMATIQUEMENT par tools/embed_pc_assets.py\n")
        f.write(f"// Ne pas editer a la main -- relancer le script si les fichiers source changent.\n")
        f.write(f'#include "embedded_assets.h"\n\n')
        entries = []
        for fname in files:
            path = os.path.join(src_dir, fname)
            if not os.path.isfile(path):
                continue
            with open(path, "rb") as fh:
                data = fh.read()
            var = "k_" + c_identifier(fname)
            emit_bytes_array(f, var, data)
            entries.append((fname, var))
        f.write(f"const {entry_type}* {table_name}( const char* name ) {{\n")
        f.write(f"    static const {entry_type} table[] = {{\n")
        for fname, var in entries:
            f.write(f'        {{ "{fname}", {var}, {var}_len }},\n')
        f.write("    };\n")
        f.write(f"    for ( const auto& e : table ) {{ if ( strcmp( e.name, name ) == 0 ) return &e; }}\n")
        f.write("    return nullptr;\n}\n")
    print(f"OK : {len(entries)} fichiers embarques dans {out_cpp} ({sum(os.path.getsize(os.path.join(src_dir,f)) for f in files if os.path.isfile(os.path.join(src_dir,f)))/1024:.1f} Ko source)")

if __name__ == "__main__":
    base = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    sdl = os.path.join(base, "platform_sdl")
    out = os.path.join(sdl, "embedded")
    os.makedirs(out, exist_ok=True)

    process_dir(os.path.join(sdl, "data"), os.path.join(out, "embedded_images.cpp"), "findEmbeddedImage", "EmbeddedAsset")
    process_dir(os.path.join(sdl, "lang"), os.path.join(out, "embedded_lang.cpp"), "findEmbeddedLang", "EmbeddedAsset")
    process_dir(os.path.join(sdl, "music"), os.path.join(out, "embedded_music.cpp"), "findEmbeddedMusic", "EmbeddedAsset")
