file(COPY directional.vertex_shader directional.fragment_shader DESTINATION shaders)
file(COPY point.vertex_shader point.fragment_shader DESTINATION shaders)
file(COPY spot.vertex_shader spot.fragment_shader DESTINATION shaders)
file(COPY scene.assbin DESTINATION model)

set(ASSIMP_DIR $ENV{ASSIMP_DIR} CACHE PATH "Assimp Directory")

find_package(ASSIMP REQUIRED)

link_directories(${ASSIMP_LIBRARY_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})

add_executable( 10-Luces            luces.cpp 
									shaders/spot.vertex_shader
									shaders/spot.fragment_shader
									shaders/point.vertex_shader
									shaders/point.fragment_shader
									shaders/directional.vertex_shader
									shaders/directional.fragment_shader
									../common/openglwindow.hpp 
									../common/openglshader.hpp
									../common/openglcamera.hpp
									../common/openglmodel.hpp )
							
target_link_libraries( 10-Luces ${GRAPHIC_LIBS} ${ASSIMP_LIBRARIES})