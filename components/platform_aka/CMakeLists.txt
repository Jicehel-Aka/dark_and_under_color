idf_component_register(
    SRCS "AkaRenderer.cpp" "AkaInput.cpp" "GeneratedAssets.cpp"
         "../../shared/game/entities/Object.cpp"
         "../../shared/game/entities/Player.cpp"
         "../../shared/game/entities/Enemy.cpp"
         "../../shared/game/levels/Level00.cpp"
         "../../shared/game/rendering/Vision.cpp"
         "../../shared/game/rendering/Minimap.cpp"
         "../../shared/game/rendering/Stats.cpp"
         "../../shared/game/rendering/Splash.cpp"
         "../../shared/game/rendering/Inventory.cpp"
         "../../shared/game/rendering/Dialogue.cpp"
         "../../shared/game/rendering/UI.cpp"
         "../../shared/game/GameApp.cpp"
    INCLUDE_DIRS "." "../../shared/platform" "../../shared/game" "../../shared/game/entities" "../../shared/game/levels" "../../shared/game/rendering"
    REQUIRES gamebuino aka_runtime)

# Meme liste de tolerances que dark_and_under_aka (code Arduboy repris
# tel quel de l'autre cote, mais ce projet-ci n'en avait jamais herite
# -- ajoutee ici pour la premiere fois, en plus des corrections faites
# directement dans le code source cette fois-ci).
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-error=maybe-uninitialized -Wno-error=unused-variable
    -Wno-error=unused-parameter -Wno-error=sign-compare
    -Wno-error=reorder -Wno-error=switch -Wno-error=parentheses
    -Wno-error=unused-function -Wno-error=missing-field-initializers)
