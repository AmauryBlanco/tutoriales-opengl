cmake_minimum_required(VERSION 3.4)

project(Tutoriales-OpenGL-Moderno)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules/" ${CMAKE_MODULE_PATH})

find_package(OpenGL REQUIRED)
find_package(GLFW3  REQUIRED)
find_package(GLEW   REQUIRED)

include_directories(${GLFW3_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})

include_directories(Common)
include_directories(Includes)

set(GRAPHIC_LIBS ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} ${GLFW3_LIBRARY})

file(GLOB tutoriales LIST_DIRECTORIES true Tutorial-[0-9]*)

foreach(tutorial ${tutoriales})
	add_subdirectory(${tutorial})
endforeach(tutorial)
