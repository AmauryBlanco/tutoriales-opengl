#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"

class Tutorial_02 : public OpenGLWindow {

public:
    Tutorial_02() : vao{}, buffer{} { }

private:
    void onstart() override {
        glPointSize(5.0f);
        shader_simple.compile("shaders/simple.vertex_shader", "shaders/simple.fragment_shader");

        // crear y enlazar un vao
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // vetices para generar un trinagulo 2D componentes (X, Y, Z = 0, W = 1)
        static const float vertex[] =
        {
             0.85f, -0.85f, 0.0f, 1.0f,
            -0.85f, -0.85f, 0.0f, 1.0f,
             0.85f,  0.85f, 0.0f, 1.0f
        };

        // componetes RGBA para los colores de cada vertice
        static const float color[] =
        {
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f
        };

        // generar dos ids para los buffer
        glGenBuffers(2, buffer);

        // enlazar el buffer de vertices
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        // almacenar datos en el buffer de vertices
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

        // describir los datos y activar vPosition (location = 0)
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        // enlazar el buffer de color
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        // almacenar datos en el buffer de color
        glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

        // describir los datos y activar vColor (location = 1)
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
    }

    void onrender(double time) override {
        glClear(GL_COLOR_BUFFER_BIT);
        shader_simple.use();

        // dibujar triangulo, usar GL_POINTS para puntos
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    OpenGLShader shader_simple;
    GLuint vao;
    GLuint buffer[2];
};

int main() {
    Tutorial_02 win_app;
    if (win_app.init("Tutoriales OpenGL Moderno", 800, 600)) {
        win_app.info();
        win_app.run();
    }
    return 0;
}