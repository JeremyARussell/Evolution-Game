cmake_minimum_required(VERSION 3.15)
project(EvolutionGame CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# ---------------------------------------------------------------------------
# Box2D
# ---------------------------------------------------------------------------
set(BOX2D_VERSION "2.3.1")
set(BOX2D_BUILD_STATIC ON  CACHE BOOL "" FORCE)
set(BOX2D_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BOX2D_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(Box2D)

# ---------------------------------------------------------------------------
# FreeGLUT  (builds target: freeglut_static)
# ---------------------------------------------------------------------------
add_subdirectory(freeglut)

# ---------------------------------------------------------------------------
# GLUI  (builds target: glui)
# ---------------------------------------------------------------------------
add_subdirectory(glui)

# ---------------------------------------------------------------------------
# SOIL  (builds target: SOIL)
# ---------------------------------------------------------------------------
add_subdirectory(SOIL)

# ---------------------------------------------------------------------------
# Evolution executable
# ---------------------------------------------------------------------------
set(EVOLUTION_SRCS
    Evolution/Framework/Main.cpp
    Evolution/Framework/Render.cpp
    Evolution/Framework/World.cpp
    Evolution/Framework/GameWorlds.cpp
    Build/Evolution/Grass.cpp
    Build/Evolution/GrassSpawner.cpp
    Build/Evolution/Ground.cpp
    Build/Evolution/MainMenu.cpp
    Build/Evolution/PowerHUD.cpp
    Build/Evolution/Seed.cpp
    Build/Evolution/Wall.cpp
    Build/Evolution/Wheeler.cpp
)

add_executable(Evolution ${EVOLUTION_SRCS})

target_include_directories(Evolution PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Evolution/Framework
    ${OPENGL_INCLUDE_DIR}
)

target_compile_definitions(Evolution PRIVATE
    _CRT_SECURE_NO_WARNINGS
    FREEGLUT_STATIC
)

target_link_libraries(Evolution
    Box2D
    freeglut_static
    glui
    SOIL
    ${OPENGL_LIBRARIES}
    glu32
    winmm
)
