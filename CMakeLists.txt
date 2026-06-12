cmake_minimum_required(VERSION 3.16)

if(APPLE AND NOT DEFINED CMAKE_OSX_ARCHITECTURES)
    set(
        CMAKE_OSX_ARCHITECTURES arm64
        CACHE STRING "Build architecture for Apple Silicon Homebrew libraries")
endif()

project(PlantsVsZombiesCppPort VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

add_executable(Game
    src/main.cpp
    src/Game.cpp
    src/resources/ResourceManager.cpp
    src/resources/Animation.cpp
    src/projectiles/Projectile.cpp
    src/plants/Plant.cpp
    src/plants/Peashooter.cpp
    src/plants/SunFlower.cpp
    src/plants/WallNut.cpp
    src/plants/CherryBomb.cpp
    src/plants/SnowPea.cpp
    src/plants/Repeater.cpp
    src/zombies/Zombie.cpp
    src/zombies/ZombieBasic.cpp
    src/zombies/ZombieConehead.cpp
    src/zombies/ZombieVariant.cpp
    src/ui/Button.cpp
    src/ui/SunToken.cpp
)

target_include_directories(Game PRIVATE src)
target_link_libraries(
    Game
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio)

add_custom_target(run
    COMMAND Game
    DEPENDS Game
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(check-upload-text
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_upload_text.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if(APPLE)
    set_target_properties(Game PROPERTIES
        MACOSX_BUNDLE FALSE
    )
endif()
