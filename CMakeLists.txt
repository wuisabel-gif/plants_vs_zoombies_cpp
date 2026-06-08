cmake_minimum_required(VERSION 3.16)

if(APPLE AND NOT DEFINED CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES arm64 CACHE STRING "Build architecture for Apple Silicon Homebrew libraries")
endif()

project(PlantsVsZombiesCppPort VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

target_include_directories(Game PRIVATE src)
target_link_libraries(Game PRIVATE SFML::Graphics SFML::Window SFML::System)

add_custom_target(run
    COMMAND Game
    DEPENDS Game
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Launching Plants vs. Zombies C++ port"
)

if(APPLE)
    set_target_properties(Game PROPERTIES
        MACOSX_BUNDLE FALSE
    )
endif()
