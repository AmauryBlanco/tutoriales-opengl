file(COPY simple.vertex_shader simple.fragment_shader DESTINATION shaders)
file(COPY cubo.obj cubo.bmp cyborg.obj cyborg.bmp DESTINATION model)

add_executable( 05-FormatoOBJ modelo_obj.cpp 
							shaders/simple.vertex_shader
							shaders/simple.fragment_shader
							../common/openglwindow.hpp 
							../common/openglshader.hpp )
							
target_link_libraries( 05-FormatoOBJ ${GRAPHIC_LIBS} )