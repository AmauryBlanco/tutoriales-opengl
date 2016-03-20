file(COPY simple.vertex_shader simple.fragment_shader DESTINATION shaders)
file(COPY lena.bmp baboon.bmp DESTINATION textures)

add_executable( Tutorial-04 texturas.cpp
							shaders/simple.vertex_shader
							shaders/simple.fragment_shader
							../common/openglwindow.hpp 
							../common/openglshader.hpp )
							
target_link_libraries( Tutorial-04 ${GRAPHIC_LIBS} )