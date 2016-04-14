file(COPY phong.vertex_shader phong.fragment_shader DESTINATION shaders)
file(COPY cessna.assbin DESTINATION model)

set(ASSIMP_DIR $ENV{ASSIMP_DIR} CACHE PATH "Assimp Directory")

find_package(ASSIMP REQUIRED)

link_directories(${ASSIMP_LIBRARY_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})

add_executable( 08-Assimp   assimp-model.cpp 
							shaders/phong.vertex_shader
							shaders/phong.fragment_shader
							../common/openglwindow.hpp 
							../common/openglshader.hpp )
							
target_link_libraries( 08-Assimp ${GRAPHIC_LIBS} ${ASSIMP_LIBRARIES})