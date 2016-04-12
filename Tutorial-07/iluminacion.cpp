#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"

#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

class Tutorial_07 : public OpenGLWindow {

public:
    Tutorial_07() : vao{}, buffer{}, textureID{}, matrixID{} {
        mv_matrix   = 0;
        view_matrix = 0;
        proj_matrix = 0;
    }

private:
    void onstart() override {

        glEnable(GL_DEPTH_TEST);

        //GLuint program = shader_simple.compile("shaders/gouraud.vertex_shader", "shaders/gouraud.fragment_shader");
        GLuint program = shader_simple.compile("shaders/phong.vertex_shader", "shaders/phong.fragment_shader");

        mv_matrix   = glGetUniformLocation(program, "mv_matrix");
        view_matrix = glGetUniformLocation(program, "view_matrix");
        proj_matrix = glGetUniformLocation(program, "proj_matrix");

        // crear y enlazar un vao
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        vector<glm::vec3> vertex;
        vector<glm::vec3> normal;
        vector<glm::vec2> uv;

        loadOBJ("model/ball.obj", vertex, normal, uv);
        //loadBMP("model/cyborg.bmp");

        vertex_count = vertex.size();

        // generar dos ids para los buffer
        glGenBuffers(3, buffer);

        // buffer de vertices
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), &vertex[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        //// buffer de textura 
        //glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        //glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2), &uv[0], GL_STATIC_DRAW);
        //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        //glEnableVertexAttribArray(1);

        // buffer de normales
        glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
        glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(glm::vec3), &normal[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);

        glClearColor(0.2f, 0.86f, 0.75f, 1.0f);
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
        glm::mat4 View = glm::lookAt(glm::vec3(3, 1.5, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

         //Establecer las matrices 
        glUniformMatrix4fv(mv_matrix, 1, GL_FALSE, glm::value_ptr(View * Model));
        glUniformMatrix4fv(view_matrix, 1, GL_FALSE, glm::value_ptr(View));
        glUniformMatrix4fv(proj_matrix, 1, GL_FALSE, glm::value_ptr(Projection));

        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    }

    // Liberar memoria usada por los buffers
    void onstop() override {
        glDeleteBuffers(3, buffer);
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
                normalIndices.push_back(n2);
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

    //GLuint loadBMP(const string& image) {
    //    ifstream file = ifstream(image.c_str(), ios::in | ios::binary);

    //    if (!file.is_open()) {
    //        cout << "No se a podido abrir la imagen: " << image << endl;
    //        return 0;
    //    }

    //    char header[54];

    //    if (file.read(header, 54)) {
    //        int dataPos = *(int*)&(header[0x0A]);
    //        int imageSize = *(int*)&(header[0x22]);
    //        int width = *(int*)&(header[0x12]);
    //        int height = *(int*)&(header[0x16]);

    //        char* data = new char[imageSize];

    //        file.read(data, imageSize);
    //        file.close();

    //        // Crear un ID para la textura OpenGL
    //        glGenTextures(1, &textureID);

    //        // Enlazar la textura recien creada
    //        // GL_TEXTURE_2D la textura es un arreglo de pixeles de 2 dimensiones
    //        glBindTexture(GL_TEXTURE_2D, textureID);

    //        // Crear la textura
    //        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    //        // Liberar memoria usada por la imagen BMP
    //        delete[] data;

    //        // filtering, wrapping
    //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //        return textureID;
    //    }

    //    return 0;
    //}


    OpenGLShader shader_simple;
    GLuint vao;
    GLuint buffer[3];
    GLuint matrixID, textureID;
    GLuint mv_matrix, view_matrix, proj_matrix;

    unsigned int vertex_count = 0;
};

int main() {
    Tutorial_07 win_app;
    if (win_app.init("OpenGL Moderno - Iluminacion", 1280, 720)) {
        win_app.info();
        win_app.run();
    }
    return 0;
}