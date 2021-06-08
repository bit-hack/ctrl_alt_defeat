cmake_minimum_required(VERSION 3.0)
project(ctrl_alt_defeat)

include_directories(libs/glfw-3.3.2/include)

add_subdirectory(libs/glfw-3.3.2)
add_subdirectory(libs/imgui)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

find_package(OpenGL REQUIRED)

add_subdirectory(rv32i_model)

add_executable(game
  source/levels.h
  source/main.cpp
  source/encoding.h
  source/encoding.cpp
  source/dasm_help.h
)
target_link_libraries(game rv32i_model imgui glfw OpenGL::GL)
