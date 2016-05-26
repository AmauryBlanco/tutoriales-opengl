project(16-Terreno)

file(COPY terrain.vs.glsl terrain.fs.glsl DESTINATION shaders)
file(COPY ../data/image/heightmap.png ../data/image/terreno.jpg DESTINATION image)

add_executable( ${PROJECT_NAME}     terrain.cpp 
									shaders/terrain.fs.glsl
									shaders/terrain.vs.glsl
									image/heightmap.png
									image/terreno.jpg
									../common/openglwindow.hpp 
									../common/openglshader.hpp
									../common/openglcamera.hpp )
							
target_link_libraries( ${PROJECT_NAME} ${GRAPHIC_LIBS})