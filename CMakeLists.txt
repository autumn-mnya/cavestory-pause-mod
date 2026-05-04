cmake_minimum_required(VERSION 3.16)

set(MOD_NAME pause)

project(${MOD_NAME} LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_PATH ${CMAKE_SOURCE_DIR}/src)
set(COMMON_PATH ${CMAKE_SOURCE_DIR}/../common)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${SRC_PATH}/*.cpp
    ${SRC_PATH}/*.c
)

file(GLOB_RECURSE COMMON_FILES CONFIGURE_DEPENDS
    ${COMMON_PATH}/*.cpp
    ${COMMON_PATH}/*.c
)

add_library(${MOD_NAME} SHARED
    ${SRC_FILES}
    ${COMMON_FILES}
)

target_include_directories(${MOD_NAME} PRIVATE
    ${SRC_PATH}
    ${COMMON_PATH}
)

target_link_libraries(${MOD_NAME} PRIVATE shlwapi)

if(MINGW)
    target_compile_options(${MOD_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
    target_link_options(${MOD_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
endif()

set_target_properties(${MOD_NAME} PROPERTIES
    OUTPUT_NAME "${MOD_NAME}"
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/CaveStory/mods/${MOD_NAME}
)
