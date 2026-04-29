add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(SOIL_SRCS
    SOIL.c
    image_DXT.c
    image_helper.c
    stb_image_aug.c
)

add_library(SOIL STATIC ${SOIL_SRCS})

target_include_directories(SOIL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
