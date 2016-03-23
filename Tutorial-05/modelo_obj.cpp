#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

class Tutorial_05 : public OpenGLWindow {

public:
    Tutorial_05() : vao{}, buffer{}, textureID{}, matrixID{} { }

private:
    void onstart() override {

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // crear, compilar y enlasar el Vertex y Fragment Shader
        GLuint program = shader_simple.compile("shaders/simple.vertex_shader", "shaders/simple.fragment_shader");

        // Obtener el ID del uniform llamado "MVP"
        matrixID = glGetUniformLocation(program, "MVP");

        // crear y enlazar un vao
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        vector<glm::vec3> vertex;
        vector<glm::vec3> normal;
        vector<glm::vec2> uv;

        loadOBJ("model/cyborg.obj", vertex, normal, uv);
        loadBMP("model/cyborg.bmp");

        vertex_count = vertex.size();

        // generar dos ids para los buffer
        glGenBuffers(2, buffer);

        // buffer de vertices
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), &vertex[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        // buffer de textura 
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2), &uv[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
    }

    void onrender(double time) override {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Usar el shader
        shader_simple.use();

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
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    }

    // Liberar memoria usada por los buffers
    void onstop() override {
        glDeleteBuffers(2, buffer);
        glDeleteBuffers(1, &textureID);
        glDeleteVertexArrays(1, &vao);
    }

    void loadOBJ(string archivo, vector<glm::vec3>& vertex, vector<glm::vec3>& normal, vector<glm::vec2>& uv) {
        ifstream file = ifstream(archivo);
        if (!file.is_open()) {
            cout << "No se ecuentra: " << archivo << endl;
            return;
        }

        vector<glm::vec3> temp_vertex;
        vector<glm::vec3> temp_normal;
        vector<glm::vec2> temp_uv;

        vector<unsigned int> vertexIndices, uvIndices, normalIndices;

        while (file) {
            string linea;
            getline(file, linea);

            istringstream str(linea);
            string start;
            str >> start;

            float v1, v2, v3;

            if (start == "vt") {
                str >> v1 >> v2;
                temp_uv.push_back(glm::vec2(v1, v2));
            }
            else if (start == "vn") {
                str >> v1 >> v2 >> v3;
                temp_normal.push_back(glm::vec3(v1, v2, v3));
            }
            else if (start == "v") {
                str >> v1 >> v2 >> v3;
                temp_vertex.push_back(glm::vec3(v1, v2, v3));
            }
            else if (start == "f") {
                unsigned int v1, v2, v3, n1, n2, n3, uv1, uv2, uv3;

                // reemplazar el caracter "/" por " "
                std::replace_if(std::begin(linea), std::end(linea), [](const char& ch) { return ch == '/'; }, ' ');

                istringstream face_str(linea);
                face_str.ignore(linea.length(), ' ');

                face_str >> v1 >> uv1 >> n1 >> v2 >> uv2 >> n2 >> v3 >> uv3 >> n3;

                vertexIndices.push_back(v1);
                vertexIndices.push_back(v2);
                vertexIndices.push_back(v3);

                normalIndices.push_back(n1);
                normalIndices.push_back(n1);
                normalIndices.push_back(n3);

                uvIndices.push_back(uv1);
                uvIndices.push_back(uv2);
                uvIndices.push_back(uv3);
            }
        }

        for (unsigned int i = 0; i < vertexIndices.size(); i++) {
            unsigned int vIndex = vertexIndices[i];
            unsigned int uvIndex = uvIndices[i];
            unsigned int nIndex = normalIndices[i];

            glm::vec3 temp_v = temp_vertex[vIndex - 1];
            glm::vec2 temp_u = temp_uv[uvIndex - 1];
            glm::vec3 temp_n = temp_normal[nIndex - 1];

            vertex.push_back(temp_v);
            uv.push_back(temp_u);
            normal.push_back(temp_n);
        }

        file.close();
    }

    GLuint loadBMP(const string& image) {
        ifstream file = ifstream(image.c_str(), ios::in | ios::binary);

        if (!file.is_open()) {
            cout << "No se a podido abrir la imagen: " << image << endl;
            return 0;
        }

        char header[54];

        if (file.read(header, 54)) {
            int dataPos = *(int*)&(header[0x0A]);
            int imageSize = *(int*)&(header[0x22]);
            int width = *(int*)&(header[0x12]);
            int height = *(int*)&(header[0x16]);

            char* data = new char[imageSize];

            file.read(data, imageSize);
            file.close();

            // Crear un ID para la textura OpenGL
            glGenTextures(1, &textureID);

            // Enlazar la textura recien creada
            // GL_TEXTURE_2D la textura es un arreglo de pixeles de 2 dimensiones
            glBindTexture(GL_TEXTURE_2D, textureID);

            // Crear la textura
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

            // Liberar memoria usada por la imagen BMP
            delete[] data;

            // filtering, wrapping
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            return textureID;
        }

        return 0;
    }


    OpenGLShader shader_simple;
    GLuint vao;
    GLuint buffer[2];
    GLuint matrixID, textureID;

    unsigned int vertex_count = 0;
};

int main() {
    Tutorial_05 win_app;
    if (win_app.init("OpenGL Moderno - Cargar Modelo 3D", 1280, 720)) {
        win_app.info();
        win_app.run();
    }
    return 0;
}