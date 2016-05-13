project(14-NormalMaps)

file(COPY normal-map.vs.glsl normal-map.fs.glsl DESTINATION shaders)
file(COPY ../Data/cyborg DESTINATION model)

set(ASSIMP_DIR $ENV{ASSIMP_DIR} CACHE PATH "Assimp Directory")

find_package(ASSIMP REQUIRED)

link_directories(${ASSIMP_LIBRARY_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})

add_executable( ${PROJECT_NAME}     normal-map.cpp 
									modelmesh.hpp
									shaders/normal-map.fs.glsl
									shaders/normal-map.vs.glsl
									../common/openglwindow.hpp 
									../common/openglshader.hpp
									../common/openglcamera.hpp )
							
target_link_libraries( ${PROJECT_NAME} ${GRAPHIC_LIBS} ${ASSIMP_LIBRARIES})