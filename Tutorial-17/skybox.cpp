#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLCamera.hpp"

#include "terrain.hpp"

#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <string>
#include <vector>

class Tutorial_OpenGL_Moderno : public OpenGLWindow {

public:
    Tutorial_OpenGL_Moderno() { }

    OpenGLShader shader;
    OpenGLCamera camera;
    Terrain terreno;

    GLuint skyboxTextureID;
    GLuint boxVAO;

private:
    void onstart() override {
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.5f, 0.5f, 1.0f);

        camera.setWindow(this->window);
        shader.compile("shaders/skybox.vs.glsl", "shaders/skybox.fs.glsl");

        terreno.init();

        std::vector<std::string> textures = {
            "image/skybox/rt.jpg",
            "image/skybox/lt.jpg",
            "image/skybox/up.jpg",
            "image/skybox/dn.jpg",
            "image/skybox/ft.jpg",
            "image/skybox/bk.jpg"
        };

        loadSkyBoxTextures(textures);
        createBox();

        glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(this->window, 1280 / 2, 720 / 2);
    }

    void loadSkyBoxTextures(std::vector<std::string> textures) {
        glGenTextures(1, &skyboxTextureID);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        for (int i = 0; i < 6; i++) {
            int channels, width, height;
          
            unsigned char *pData = stbi_load(textures[i].c_str(), &width, &height, &channels, STBI_rgb);

            if (pData == nullptr) cout << "Error al cargar: " << textures[i] << endl;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
            stbi_image_free(pData);
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void createBox() {
        glGenVertexArrays(1, &boxVAO);
        glBindVertexArray(boxVAO);

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

        // indices usados para unir los vertices que componen el cubo
        static const GLushort indices[] =
        {
            0, 1, 2, 3, 0, 2,
            4, 5, 1, 0, 4, 1,
            4, 7, 5, 5, 6, 2,
            6, 7, 3, 0, 3, 7,        
            7, 6, 5, 1, 5, 2,
            2, 6, 3, 4, 0, 7
        };

        GLuint index_buffer = 0;
        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        GLuint buffer = 0;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void onrender(double time) override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        terreno.render(camera);

        shader.use();
        
        glm::mat4 M;
        M = glm::translate(M, camera.getPosition());
        M = glm::scale(M, glm::vec3(100.0f));

        glm::mat4 MV = camera.getViewMatrix() * M;
        glm::mat4 MVP = camera.getProjectionMatrix() * MV;

        glUniformMatrix4fv(shader.getUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniformMatrix4fv(shader.getUniformLocation("MV"), 1, GL_FALSE, glm::value_ptr(MV));

        glActiveTexture(GL_TEXTURE0);        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);

        glBindVertexArray(boxVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
 
        shader.unUse();
    }

    void onkey(int key, int scancode, int action, int mods) override {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }

};

int main() {
    Tutorial_OpenGL_Moderno win_app;

    if (win_app.init("OpenGL Moderno - SkyBox", 1280, 720)) {
        win_app.info();

        cout << "TOTURIALES OPENGL MODERNO - SKYBOX" << endl << endl;
        cout << "Usar la tecla [Esc] para salir." << endl << endl;

        win_app.run();
    }

    return 0;
}