file(COPY simple.vertex_shader simple.fragment_shader DESTINATION shaders)

add_executable( Tutorial-01 shader-opengl.cpp 
							shaders/simple.vertex_shader
							shaders/simple.fragment_shader
							../common/openglwindow.hpp )
							
target_link_libraries( Tutorial-01 ${GRAPHIC_LIBS} )