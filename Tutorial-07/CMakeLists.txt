file(COPY gouraud.vertex_shader gouraud.fragment_shader DESTINATION shaders)
file(COPY phong.vertex_shader phong.fragment_shader DESTINATION shaders)
file(COPY cubo.obj ball.obj DESTINATION model)

add_executable( 07-Iluminacion_phong iluminacion.cpp 
							shaders/gouraud.vertex_shader
							shaders/gouraud.fragment_shader
							shaders/phong.vertex_shader
							shaders/phong.fragment_shader
							../common/openglwindow.hpp 
							../common/openglshader.hpp )
							
target_link_libraries( 07-Iluminacion_phong ${GRAPHIC_LIBS} )