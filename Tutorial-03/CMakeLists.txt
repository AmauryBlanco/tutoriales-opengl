file(COPY simple.vertex_shader simple.fragment_shader DESTINATION shaders)

add_executable( Tutorial-03 3d-opengl.cpp 
							shaders/simple.vertex_shader
							shaders/simple.fragment_shader
							../common/openglwindow.hpp 
							../common/openglshader.hpp )
							
target_link_libraries( Tutorial-03 ${GRAPHIC_LIBS} )