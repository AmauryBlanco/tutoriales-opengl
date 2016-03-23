cmake_minimum_required(VERSION 3.4)

project(Tutoriales-OpenGL-Moderno)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules/" ${CMAKE_MODULE_PATH})

find_package(OpenGL REQUIRED)
find_package(GLFW3  REQUIRED)
find_package(GLEW   REQUIRED)
find_package(GLM    REQUIRED)

include_directories(${GLFW3_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(Common)

set(GRAPHIC_LIBS ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} ${GLFW3_LIBRARY})

add_subdirectory(Tutorial-00)
add_subdirectory(Tutorial-01)
add_subdirectory(Tutorial-02)
add_subdirectory(Tutorial-03)
add_subdirectory(Tutorial-04)
add_subdirectory(Tutorial-05)
