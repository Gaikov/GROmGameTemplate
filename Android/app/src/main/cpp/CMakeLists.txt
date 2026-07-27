# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)

project("grom-game-mobile")

# Creates your game shared library. The name must be the same as the
# one used for loading in your Kotlin/Java or AndroidManifest.txt files.

add_library(grom-game-mobile SHARED
        main.cpp
        Renderer.cpp
)

# Searches for a package provided by the game activity dependency
find_package(game-activity REQUIRED CONFIG)
find_package(games-frame-pacing REQUIRED CONFIG)

# Configure libraries CMake uses to link your target library.
target_link_libraries(grom-game-mobile
        # The game activity
        -Wl,--whole-archive
        game-activity::game-activity_static
        -Wl,--no-whole-archive
        games-frame-pacing::swappy_static

        # EGL and other dependent libraries required for drawing
        # and interacting with Android system

        EGL
        jnigraphics
        android
        AndroidPlatform
        log
        ${GAME_MODULE_NAME}
)
