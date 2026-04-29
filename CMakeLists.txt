cmake_minimum_required(VERSION 3.20)

project(raytracing)

file(GLOB_RECURSE ARQUIVOS
     CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(raytracing ${ARQUIVOS})
target_include_directories(raytracing PRIVATE "${CMAKE_SOURCE_DIR}/src/")