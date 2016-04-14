file(COPY simple.vertex_shader simple.fragment_shader DESTINATION shaders)

add_executable( 02-VertexBufferObject data-opengl.cpp 
							shaders/simple.vertex_shader
							shaders/simple.fragment_shader
							../common/openglwindow.hpp
							../common/openglshader.hpp )
							
target_link_libraries( 02-VertexBufferObject ${GRAPHIC_LIBS} )