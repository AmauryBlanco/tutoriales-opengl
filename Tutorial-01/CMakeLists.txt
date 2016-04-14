file(COPY simple.vertex_shader simple.fragment_shader DESTINATION shaders)

add_executable( 01-Shader shader-opengl.cpp 
							shaders/simple.vertex_shader
							shaders/simple.fragment_shader
							../common/openglwindow.hpp
							../common/openglshader.hpp )
							
target_link_libraries( 01-Shader ${GRAPHIC_LIBS} )