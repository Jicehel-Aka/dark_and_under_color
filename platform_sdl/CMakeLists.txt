cmake_minimum_required(VERSION 3.16)
project(dark_and_under_color_pc CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(darkandundercolor_pc
    main.cpp
    SdlRenderer.cpp
    SdlInput.cpp
    Assets.cpp
    ../shared/game/entities/Object.cpp
    ../shared/game/entities/Player.cpp
    ../shared/game/entities/Enemy.cpp
    ../shared/game/levels/Level00.cpp
    ../shared/game/rendering/Vision.cpp
    ../shared/game/rendering/Minimap.cpp
    ../shared/game/rendering/Stats.cpp
    ../shared/game/rendering/Splash.cpp
    ../shared/game/rendering/Inventory.cpp
    ../shared/game/rendering/Dialogue.cpp
    ../shared/game/rendering/UI.cpp
    SdlTranslator.cpp
    ../shared/game/GameApp.cpp
    embedded/embedded_images.cpp
    embedded/embedded_lang.cpp
)

target_include_directories(darkandundercolor_pc PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ../shared/platform
    ../shared/game
    ../shared/game/entities
    ../shared/game/levels
    ../shared/game/rendering
    ../components/aka_font/include
    .
)

# BUG TROUVE ET CORRIGE (retour de Jicehel, log de lien Windows a
# l'appui -- "LNK1181: cannot open input file 'SDL2_image.lib'") : les
# noms bruts (SDL2_image, SDL2_mixer) marchaient sur Linux PAR
# COINCIDENCE -- CMake, ne les reconnaissant pas comme des cibles,
# retombe sur la convention "-lSDL2_image" du lieur Linux, qui trouve
# la bibliotheque via les chemins systeme standards. Windows/MSVC n'a
# pas cette convention : il lui faut soit un fichier .lib exact dans
# son chemin de recherche, soit la vraie cible CMake exportee par
# vcpkg (qui, elle, sait ou se trouve precisement ce .lib). Utilise
# les cibles modernes (SDL2::SDL2, etc.) quand elles existent
# (garanti par vcpkg sous Windows), sinon retombe sur l'ancienne
# methode par variables (qui fonctionnait deja sous Linux -- pas de
# risque de regression la ou ca marchait).
if ( TARGET SDL2_image::SDL2_image )
    set( SDL2_IMAGE_LINK_TARGET SDL2_image::SDL2_image )
else()
    set( SDL2_IMAGE_LINK_TARGET SDL2_image )
endif()

if ( TARGET SDL2_mixer::SDL2_mixer )
    set( SDL2_MIXER_LINK_TARGET SDL2_mixer::SDL2_mixer )
else()
    set( SDL2_MIXER_LINK_TARGET SDL2_mixer )
endif()

if ( TARGET SDL2::SDL2 )
    target_link_libraries(darkandundercolor_pc PRIVATE SDL2::SDL2 ${SDL2_IMAGE_LINK_TARGET} ${SDL2_MIXER_LINK_TARGET})
    if ( TARGET SDL2::SDL2main )
        target_link_libraries(darkandundercolor_pc PRIVATE SDL2::SDL2main)
    endif()
else()
    target_link_libraries(darkandundercolor_pc PRIVATE ${SDL2_LIBRARIES} ${SDL2_IMAGE_LINK_TARGET} ${SDL2_MIXER_LINK_TARGET})
endif()
