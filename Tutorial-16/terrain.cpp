#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLCamera.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb\stb_image.h"

#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <string>
#include <vector>

class Tutorial_OpenGL_Moderno : public OpenGLWindow {

public:
    Tutorial_OpenGL_Moderno() { }

    OpenGLCamera camera;
    OpenGLShader shader;

    GLuint terrainTextureID;
    GLuint vao, indices_count;
    GLuint buffer[2];

private:
    void onstart() override {
        glEnable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glClearColor(0.25f, 0.80f, 0.75f, 1.0f);

        camera.setWindow(this->window);
        shader.compile("shaders/terrain.vs.glsl", "shaders/terrain.fs.glsl");

        std::vector<GLubyte> altura;
        int texture_width, texture_height;
        loadTerrainHeight("image/heightmap.png", altura, texture_width, texture_height);
        createPlainTerrain(texture_width, texture_height, altura);

        terrainTextureID = loadTerrainTexture("image/terreno.jpg");

        glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(this->window, 1280 / 2, 720 / 2);
    }

    void loadTerrainHeight(const std::string& filename, std::vector<GLubyte>& altura, int& width, int& height)
    {
        int channels;
        unsigned char *pData = stbi_load(filename.c_str(), &width, &height, &channels, STBI_grey);

        if (pData == nullptr) {
            std::cout << "Error al cargar: " << filename << std::endl;
            return;
        }

        altura.insert(altura.end(), pData, pData + (width * height));
        stbi_image_free(pData);
    }


    GLuint loadTerrainTexture(const std::string& filename)
    {
        GLuint textureID = 0;
        glGenTextures(1, &textureID);

        int channels, width, height;
        unsigned char *pData = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb);

        if (pData == nullptr) std::cout << "Error al cargar: " << filename << std::endl;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(pData);

        return textureID;
    }

    void createPlainTerrain(int WIDTH, int DEPTH, std::vector<GLubyte>& altura) {

        std::vector<glm::vec3> vertices;
        vertices.reserve(DEPTH * WIDTH);

        int counter = 0;

        for (int j = 0; j < DEPTH; j++) {
            for (int i = 0; i < WIDTH; i++) {
                float x = (float(i) / (WIDTH - 1)) * 2.0f - 1.0f;
                float z = (float(j) / (DEPTH - 1)) * 2.0f - 1.0f;
                float y = altura[counter++] / 255.0f;

                vertices.push_back(glm::vec3(x, y, z));
            }
        }

        std::vector<GLuint> indices((DEPTH - 1) * (WIDTH - 1) * 6);
        std::vector<GLuint>::iterator id;
        id = indices.begin();

        for (int z = 0; z < DEPTH - 1; z++) {
            for (int x = 0; x < WIDTH - 1; x++) {
                int i0 = (z * DEPTH) + x;
                int i1 = i0 + 1;
                int i2 = ((z + 1) * DEPTH) + x;
                int i3 = i2 + 1;

                *id++ = i0; *id++ = i2; *id++ = i1;
                *id++ = i1; *id++ = i2; *id++ = i3;
            }
        }

        indices_count = indices.size();

        std::vector<glm::vec3> normales(DEPTH * WIDTH);
        calculateNormals(normales, vertices, indices);

        glGenVertexArrays(1, &vao);
        glGenBuffers(2, buffer);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        GLuint buffer_normal = 0;
        glGenBuffers(1, &buffer_normal);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_normal);
        glBufferData(GL_ARRAY_BUFFER, normales.size() * sizeof(glm::vec3), &normales[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void calculateNormals(vector<glm::vec3>& m_normals, vector<glm::vec3>& m_vertices, vector<GLuint>& m_indices)
    {
        for (size_t i = 0; i < m_indices.size(); i += 3)
        {
            glm::vec3 p0 = m_vertices[m_indices[i + 0]];
            glm::vec3 p1 = m_vertices[m_indices[i + 1]];
            glm::vec3 p2 = m_vertices[m_indices[i + 2]];

            glm::vec3 e1 = p0 - p1;
            glm::vec3 e2 = p0 - p2;
            glm::vec3 normal = glm::normalize(glm::cross(e1, e2));

            m_normals[m_indices[i + 0]] += normal;
            m_normals[m_indices[i + 1]] += normal;
            m_normals[m_indices[i + 2]] += normal;
        }

        for (size_t i = 0; i < m_normals.size(); ++i)
            m_normals[i] = glm::normalize(m_normals[i]);
    }

    void onrender(double time) override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 M = glm::mat4(1);
        glm::mat4 MV = camera.getViewMatrix() * M;
        glm::mat4 MVP = camera.getProjectionMatrix() * MV;
        glm::mat3 N = glm::inverseTranspose(glm::mat3(MV));

        glUniformMatrix4fv(shader.getUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniformMatrix4fv(shader.getUniformLocation("MV"), 1, GL_FALSE, glm::value_ptr(MV));
        glUniformMatrix3fv(shader.getUniformLocation("N"), 1, GL_FALSE, glm::value_ptr(N));

        glm::vec3 lightPosES = glm::vec3(MV * glm::vec4(10, 10, 10, 0));

        glUniform3fv(shader.getUniformLocation("light_position"), 1, glm::value_ptr(lightPosES));
        glUniform3fv(shader.getUniformLocation("diffuse_color"), 1, glm::value_ptr(glm::vec3(0.75f)));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        shader.unUse();
    }

    void onkey(int key, int scancode, int action, int mods) override {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }

};

int main() {
    Tutorial_OpenGL_Moderno win_app;

    if (win_app.init("OpenGL Moderno - Terreno", 1280, 720)) {
        win_app.info();

        cout << "TOTURIALES OPENGL MODERNO - TERRENO" << endl << endl;
        cout << "Usar la tecla [Esc] para salir." << endl << endl;

        win_app.run();
    }

    return 0;
}