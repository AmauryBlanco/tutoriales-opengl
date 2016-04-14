file(COPY simple.vertex_shader simple.fragment_shader DESTINATION shaders)
file(COPY lena.bmp baboon.bmp DESTINATION textures)

add_executable( 04-Texturas texturas.cpp
							shaders/simple.vertex_shader
							shaders/simple.fragment_shader
							../common/openglwindow.hpp 
							../common/openglshader.hpp )
							
target_link_libraries( 04-Texturas ${GRAPHIC_LIBS} )