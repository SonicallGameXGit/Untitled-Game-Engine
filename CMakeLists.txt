cmake_minimum_required(VERSION 3.10)
project(GLEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG master
)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_Declare(
    sdl
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.18
)
FetchContent_Declare(
    sdl_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.2.4
)
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.15.0
)
FetchContent_Declare(
    msdf_atlas_gen
    GIT_REPOSITORY https://github.com/Chlumsky/msdf-atlas-gen.git
    GIT_TAG v1.3
)

set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "" FORCE)
set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "" FORCE)
set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "" FORCE)
set(MSDF_ATLAS_NO_ARTERY_FONT ON CACHE BOOL "" FORCE)
set(MSDF_ATLAS_INSTALL ON CACHE BOOL "" FORCE)
set(GLM_ENABLE_SIMD_AVX ON CACHE BOOL "" FORCE)
set(ONLY_LIBS ON CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glad glm sdl sdl_image entt msdf_atlas_gen)

add_executable(${PROJECT_NAME}
    src/test.cpp

    src/framework/util/debug.cpp
    src/framework/graphics/window.cpp
    src/framework/graphics/shader.cpp
    src/framework/graphics/buffers.cpp
    src/framework/graphics/texture.cpp

    src/engine/world/camera.cpp
    src/engine/world/mesh.cpp
    
    src/engine/graphics/gui/text.cpp
    src/engine/graphics/renderer.cpp
    
    src/engine/ecs/ecs.cpp
    src/engine/ecs/components/transform.cpp
    src/engine/ecs/components/mesh.cpp
    src/engine/ecs/components/gui.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${glad_BINARY_DIR}/include
    ${glm_SOURCE_DIR}
    ${sdl_SOURCE_DIR}/include
    ${sdl_image_SOURCE_DIR}/include
    ${entt_SOURCE_DIR}/single_include
    ${msdf_atlas_gen_SOURCE_DIR}
    ${msdf_atlas_gen_SOURCE_DIR}/msdfgen
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    glad
    glm::glm
    SDL3::SDL3
    SDL3_image::SDL3_image
    msdf-atlas-gen::msdf-atlas-gen
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
