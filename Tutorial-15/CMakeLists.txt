project(15-Sombras)

file(COPY shadows.vs.glsl shadows.fs.glsl shadow-map-debug.vs.glsl shadow-map-debug.fs.glsl shadow-map.vs.glsl shadow-map.fs.glsl DESTINATION shaders)
file(COPY ../Data/test.assbin DESTINATION model)

set(ASSIMP_DIR $ENV{ASSIMP_DIR} CACHE PATH "Assimp Directory")

find_package(ASSIMP REQUIRED)

link_directories(${ASSIMP_LIBRARY_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})

add_executable( ${PROJECT_NAME}     shadow-map.cpp 
									shaders/shadow-map.fs.glsl
									shaders/shadow-map.vs.glsl
									shaders/shadow-map-debug.fs.glsl
									shaders/shadow-map-debug.vs.glsl
									shaders/shadows.fs.glsl
									shaders/shadows.vs.glsl
									../common/openglmodel.hpp
									../common/openglwindow.hpp 
									../common/openglshader.hpp
									../common/openglcamera.hpp )
							
target_link_libraries( ${PROJECT_NAME} ${GRAPHIC_LIBS} ${ASSIMP_LIBRARIES})