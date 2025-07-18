cmake_minimum_required(VERSION 3.10)
project(GLEngine)

find_package(OpenGL REQUIRED)
find_package(GLEW QUIET 2.2.0)
find_package(GLM QUIET 1.0.1)
find_package(SDL3 QUIET 3.2.18)
find_package(SDL3_image QUIET 3.2.4)

if(NOT GLEW_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        glew
        GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
        GIT_TAG glew-cmake-2.2.0
    )
    FetchContent_MakeAvailable(glew)
    FetchContent_GetProperties(glew)

    set(GLEW_INCLUDE_DIRS ${glew_SOURCE_DIR}/include)
    set(GLEW_LIBRARIES ${glew_SOURCE_DIR}/lib/libGLEW.a)
    set(GLEW_FOUND TRUE)
else()
    message(STATUS "Using existing GLEW installation")
    set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
    set(GLEW_LIBRARIES ${GLEW_LIBRARY})
endif()

if(NOT GLM_FOUND)
    include(FetchContent)
    option(GLM_ENABLE_SIMD_AVX "Enable AVX optimizations" ON)

    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
    )
    FetchContent_MakeAvailable(glm)
    FetchContent_GetProperties(glm)

    set(GLM_INCLUDE_DIRS ${glm_SOURCE_DIR}/glm)
    set(GLM_LIBRARIES ${glm_SOURCE_DIR}/lib/libglm.a)
    set(GLM_FOUND TRUE)
else()
    message(STATUS "Using existing GLM installation")
    set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIR})
    set(GLM_LIBRARIES ${GLM_LIBRARY})
endif()

if(NOT SDL_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        SDL
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.18
    )
    FetchContent_MakeAvailable(SDL)
    FetchContent_GetProperties(SDL)

    set(SDL_INCLUDE_DIRS ${SDL_SOURCE_DIR}/include)
    set(SDL_LIBRARIES ${SDL_LIBRARY})
    set(SDL_FOUND TRUE)
else()
    message(STATUS "Using existing SDL installation")
    set(SDL_INCLUDE_DIRS ${SDL_INCLUDE_DIR})
    set(SDL_LIBRARIES ${SDL_LIBRARY})
endif()

if(NOT SDL_image_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        SDL_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.2.4
    )
    FetchContent_MakeAvailable(SDL_image)
    FetchContent_GetProperties(SDL_image)
    
    set(SDL_image_INCLUDE_DIRS ${SDL_image_SOURCE_DIR}/include)
    set(SDL_image_LIBRARIES ${SDL_image_LIBRARY})
    set(SDL_image_FOUND TRUE)
else()
    message(STATUS "Using existing SDL_image installation")
    set(SDL_image_INCLUDE_DIRS ${SDL_image_INCLUDE_DIR})
    set(SDL_image_LIBRARIES ${SDL_image_LIBRARY})
endif()

add_executable(${PROJECT_NAME}
    src/test.cpp
    src/graphics/window.cpp
    src/graphics/shader.cpp
    src/graphics/mesh.cpp
    src/graphics/texture.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLEW_INCLUDE_DIRS}
    ${SDL_INCLUDE_DIRS}
    ${SDL_image_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    SDL3::SDL3
    SDL3_image::SDL3_image
    GLEW::glew
    glm::glm
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
