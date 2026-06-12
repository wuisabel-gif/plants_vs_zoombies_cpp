cmake_minimum_required(VERSION 3.16)

project(PlantsVsZombiesWebPreview LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(web_preview main.cpp)

target_compile_options(web_preview PRIVATE -sUSE_SDL=2)
target_link_options(web_preview PRIVATE
    -sUSE_SDL=2
    -sALLOW_MEMORY_GROWTH=1
    -sASSERTIONS=1
    --shell-file=${CMAKE_CURRENT_SOURCE_DIR}/shell.html
)

set_target_properties(web_preview PROPERTIES
    OUTPUT_NAME "index"
    SUFFIX ".html"
)
