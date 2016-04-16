file(COPY phong.vertex_shader phong.fragment_shader DESTINATION shaders)
file(COPY grid.assbin DESTINATION model)

set(ASSIMP_DIR $ENV{ASSIMP_DIR} CACHE PATH "Assimp Directory")

find_package(ASSIMP REQUIRED)

link_directories(${ASSIMP_LIBRARY_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})

add_executable( 09-Camara   camara.cpp 
							shaders/phong.vertex_shader
							shaders/phong.fragment_shader
							../common/openglwindow.hpp 
							../common/openglshader.hpp )
							
target_link_libraries( 09-Camara ${GRAPHIC_LIBS} ${ASSIMP_LIBRARIES})