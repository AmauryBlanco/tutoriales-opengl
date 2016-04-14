file(COPY simple.vertex_shader simple.fragment_shader DESTINATION shaders)

add_executable( 03-Render3D 3d-opengl.cpp 
							shaders/simple.vertex_shader
							shaders/simple.fragment_shader
							../common/openglwindow.hpp 
							../common/openglshader.hpp )
							
target_link_libraries( 03-Render3D ${GRAPHIC_LIBS} )