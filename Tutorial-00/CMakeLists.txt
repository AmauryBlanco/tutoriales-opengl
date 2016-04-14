add_executable( 00-Ventana ventana-opengl.cpp ../common/openglwindow.hpp)

target_link_libraries( 00-Ventana ${GRAPHIC_LIBS} )