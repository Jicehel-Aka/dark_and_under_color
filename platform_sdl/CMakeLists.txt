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
)

target_include_directories(darkandundercolor_pc PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ../shared/platform
    ../shared/game
    ../shared/game/entities
    ../shared/game/levels
    ../shared/game/rendering
    .
)

target_link_libraries(darkandundercolor_pc PRIVATE ${SDL2_LIBRARIES} SDL2_image SDL2_mixer)
