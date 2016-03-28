#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLText.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>

class Tutorial_03 : public OpenGLWindow {

public:
    Tutorial_03() : vao{}, buffer{}, index_buffer{}, MatrixID{} { }

private:
    void onstart() override {

        glEnable(GL_DEPTH_TEST);

        // crear, compilar y enlasar el Vertex y Fragment Shader
        GLuint program = shader_simple.compile("shaders/simple.vertex_shader", "shaders/simple.fragment_shader");

        // Obtener el ID del uniform llamado "MVP"
        MatrixID = glGetUniformLocation(program, "MVP");

        // crear y enlazar un vao
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // vetices para generar un cubo 3D
        static const float vertex[] =
        {
             1.0f, -1.0f, -1.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 1.0f,
            -1.0f, -1.0f,  1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f, 1.0f,
             1.0f,  1.0f, -1.0f, 1.0f,
             1.0f,  1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f, -1.0f, 1.0f
        };

        // componetes RGBA para los colores de cada vertice
        static const float color[] =
        {
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f
        };

        // indices usados para unir los vertices que componen el cubo
        static const GLushort indices[] =
        {
            0, 1, 2,
            7, 6, 5,
            4, 5, 1,
            5, 6, 2,
            6, 7, 3,
            0, 3, 7,
            3, 0, 2,
            4, 7, 5,
            0, 4, 1,
            1, 5, 2,
            2, 6, 3,
            4, 0, 7
        };

        // generar, almacenar el buffer de indices 
        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

        // Inicializar Fuente TrueType
        text.loadFont("font/roboto.ttf", 48);
        text.setSize(1280, 768);    
    }

    void onrender(double time) override {
        glClearColor(0.25f, 0.55f, 0.55f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Usar el shader
        shader_simple.use();
        glBindVertexArray(vao);

        // Matriz de modelo, se aplica una rotacion sobre el eje Y 
        glm::mat4 Model;
        Model = glm::rotate(Model, (float)time, glm::vec3(0.0f, 1.0f, 0.0f));

        // Matriz de proyeccion y visualizacion
        glm::mat4 Projection = glm::perspective(45.0f, aspect_ratio, 0.1f, 100.0f);
        glm::mat4 View = glm::lookAt(glm::vec3(4, 3, -3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        // Crear la matriz total MVP
        glm::mat4 MVP = Projection * View * Model;

        // Enviar las tranformaciones al shader
        // MatrixID es el ID del uniform MVP obtenida por glGetUniformLocation(program, "MVP");
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Dibujar cubo usando los indices almacenados en el buffer,
        // 1 triangulo = 3 indices, 1 cara = 2 triangulos, 1 cubo = 6 caras.
        // 3 * 2 * 6 = 36 indices a dibujar 
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

        glBindVertexArray(0);
        shader_simple.unUse();

        // Dibujar Texto 2D
        draw_text(time);
    }

    void draw_text(double time) {
        std::wostringstream str;
        str.precision(2);
        str.setf(std::ios::fixed);
        str << "Time: " << time << " ms.";

        // Establecer color y dibujar texto en la ubicacion indicada.
        text.setColor(0.15f, 0.3f, 0.7f);
        text.draw(10, 60, L"Tutor de Programación - Render Text 2D");

        // Establecer color y dibujar texto en la ubicacion indicada.
        text.setColor(0.3f, 0.3f, 0.3f);
        text.draw(10, 120, str.str());
    }

    // Liberar memoria usada por los buffers
    void onstop() override {
        glDeleteBuffers(2, buffer);
        glDeleteBuffers(1, &index_buffer);
        glDeleteVertexArrays(1, &vao);
    }

    OpenGLText text;
    OpenGLShader shader_simple;
    GLuint vao, index_buffer;
    GLuint buffer[2];
    GLuint MatrixID;
};

int main() {
    Tutorial_03 win_app;
    if (win_app.init("OpenGL Moderno - Render Text 2D", 1280, 720)) {
        win_app.info();
        win_app.run();
    }
    return 0;
}