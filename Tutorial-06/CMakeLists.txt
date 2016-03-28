file(COPY simple.vertex_shader simple.fragment_shader DESTINATION shaders)
file(COPY text.vertex_shader text.fragment_shader DESTINATION shaders)
file(COPY roboto.ttf DESTINATION font)

include_directories(../includes)

add_executable( Tutorial-06 texto-opengl.cpp 
							shaders/simple.vertex_shader
							shaders/simple.fragment_shader
			                shaders/text.vertex_shader
							shaders/text.fragment_shader
							../common/openglwindow.hpp 
							../common/openglshader.hpp
							../common/opengltext.hpp )
							
target_link_libraries( Tutorial-06 ${GRAPHIC_LIBS} )