project(17-SkyBox)

file(COPY skybox.vs.glsl terrain.vs.glsl terrain.fs.glsl skybox.fs.glsl DESTINATION shaders)
file(COPY ../data/image/skybox DESTINATION image)
file(COPY ../data/image/heightmap.png ../data/image/terreno.jpg DESTINATION image)

add_executable( ${PROJECT_NAME}     skybox.cpp 
									terrain.hpp
									shaders/skybox.fs.glsl
									shaders/skybox.vs.glsl
									../common/openglwindow.hpp 
									../common/openglshader.hpp
									../common/openglcamera.hpp )
							
target_link_libraries( ${PROJECT_NAME} ${GRAPHIC_LIBS} )