#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"

class Tutorial_01 : public OpenGLWindow {

    void onstart() override {
        // aumentar tamaño del punto
        glPointSize(15.0f);
        // crear el vertex y fragment shader
        shader_simple.compile("shaders/simple.vertex_shader", "shaders/simple.fragment_shader");
    }

    void onrender(double time) override {
        // limpiar el buffer
        glClear(GL_COLOR_BUFFER_BIT);
        // usar el shader 
        shader_simple.use();
        //dibujar el punto en pantalla
        glDrawArrays(GL_POINTS, 0, 1);
    }

    OpenGLShader shader_simple;
};

int main() {
    Tutorial_01 win_app;
    if (win_app.init("Tutoriales OpenGL Moderno", 800, 600)) {
        win_app.info();
        win_app.run();
    }
    return 0;
}