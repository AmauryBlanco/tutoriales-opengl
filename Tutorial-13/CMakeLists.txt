project(13-Assimp_Texturas)

file(COPY directional.vertex_shader directional.fragment_shader DESTINATION shaders)
file(COPY ../Data/cyborg DESTINATION model)

set(ASSIMP_DIR $ENV{ASSIMP_DIR} CACHE PATH "Assimp Directory")

find_package(ASSIMP REQUIRED)

link_directories(${ASSIMP_LIBRARY_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})

add_executable( ${PROJECT_NAME}     texturas.cpp 
									modelmesh.hpp
									shaders/directional.vertex_shader
									shaders/directional.fragment_shader
									../common/openglwindow.hpp 
									../common/openglshader.hpp
									../common/openglcamera.hpp )
							
target_link_libraries( ${PROJECT_NAME} ${GRAPHIC_LIBS} ${ASSIMP_LIBRARIES})