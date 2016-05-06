project(12-Assimp_Materiales)

file(COPY directional.vertex_shader directional.fragment_shader DESTINATION shaders)
file(COPY test.assbin DESTINATION model)

set(ASSIMP_DIR $ENV{ASSIMP_DIR} CACHE PATH "Assimp Directory")

find_package(ASSIMP REQUIRED)

link_directories(${ASSIMP_LIBRARY_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})

add_executable( ${PROJECT_NAME}     materiales.cpp 
									shaders/directional.vertex_shader
									shaders/directional.fragment_shader
									../common/openglwindow.hpp 
									../common/openglshader.hpp
									../common/openglcamera.hpp
									../common/openglmodel.hpp )
							
target_link_libraries( ${PROJECT_NAME} ${GRAPHIC_LIBS} ${ASSIMP_LIBRARIES})