#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLCamera.hpp"
#include "OpenGLModel.hpp"

#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <string>

class Tutorial_OpenGL_Moderno : public OpenGLWindow {

public:
    Tutorial_OpenGL_Moderno() { }

private:
    void onstart() override {
        glEnable(GL_DEPTH_TEST);

        glClearDepth(1.0f);
        glClearColor(0.0f, 0.5f, 0.5f, 1.0f);

        camera.setWindow(this->window);

        light_shadow_shader.compile("shaders/shadows.vs.glsl", "shaders/shadows.fs.glsl");
        shadow_shader.compile("shaders/shadow-map.vs.glsl", "shaders/shadow-map.fs.glsl");
        debug_shader.compile("shaders/shadow-map-debug.vs.glsl", "shaders/shadow-map-debug.fs.glsl");

        model.init("model/test.assbin");

        createShadowMap();

        // ocultar el cursor y ubicarlo en el centro de la ventana
        glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(this->window, 1280 / 2, 720 / 2);
    }

    void createShadowMap() {
        glGenTextures(1, &shadowMapTexID);
        glBindTexture(GL_TEXTURE_2D, shadowMapTexID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        static GLfloat border[4] = { 1, 0, 0, 0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        glGenFramebuffers(1, &fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapTexID, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            cout << "Error al inicializar FBO." << endl;
        }

        glDrawBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void onrender(double time) override {

        if (showDepthMap) glDisable(GL_CULL_FACE);
        else glEnable(GL_CULL_FACE);

        shadow_shader.use();

        float X = sin(static_cast<float>(time)) * 10.0f;
        float Z = cos(static_cast<float>(time)) * 10.0f;

        glm::vec3 lightPosOS(X, 8.0f, Z);

        glm::mat4 P_L = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
        glm::mat4 MV_L = glm::lookAt(lightPosOS, glm::vec3(0), glm::vec3(0, 1, 0));

        glUniformMatrix4fv(shadow_shader.getUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(P_L * MV_L));

        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
        glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);

        model.draw();
        
        glDisable(GL_POLYGON_OFFSET_FILL);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shadow_shader.unUse();


        // Restablecer viewport para dibujar de modo normal
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Visualizar mapa de profundidad
        if (showDepthMap) {
            debug_shader.use();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
            RenderQuad();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            debug_shader.unUse();
        }

        light_shadow_shader.use();

        glm::mat4 B(0.5, 0.0, 0.0, 0.0,
                    0.0, 0.5, 0.0, 0.0,
                    0.0, 0.0, 0.5, 0.0,
                    0.5, 0.5, 0.5, 1.0);

        glm::mat4 S = B * P_L * MV_L;

        glm::mat4 M = glm::mat4(1);
        glm::mat4 MV = camera.getViewMatrix() * M;
        glm::mat4 MVP = camera.getProjectionMatrix() * MV;
        glm::mat3 N = glm::inverseTranspose(glm::mat3(MV));

        glUniformMatrix4fv(light_shadow_shader.getUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniformMatrix4fv(light_shadow_shader.getUniformLocation("MV"), 1, GL_FALSE, glm::value_ptr(MV));
        glUniformMatrix4fv(light_shadow_shader.getUniformLocation("M"), 1, GL_FALSE, glm::value_ptr(M));
        glUniformMatrix4fv(light_shadow_shader.getUniformLocation("S"), 1, GL_FALSE, glm::value_ptr(S));
        glUniformMatrix3fv(light_shadow_shader.getUniformLocation("N"), 1, GL_FALSE, glm::value_ptr(N));

        glm::vec3 lightPosES = glm::vec3(MV * glm::vec4(lightPosOS, 0));

        glUniform3fv(light_shadow_shader.getUniformLocation("light_position"), 1, glm::value_ptr(lightPosES));
        glUniform3fv(light_shadow_shader.getUniformLocation("diffuse_color"), 1, glm::value_ptr(glm::vec3(0.75f)));

        model.draw();

        light_shadow_shader.unUse();

    }

    GLuint quadVAO = 0;
    GLuint quadVBO;
    void RenderQuad()
    {
        if (quadVAO == 0)
        {
            GLfloat quadVertices[] = {
                // Positions      // Texture Coords
                -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
            };

            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        }

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void onkey(int key, int scancode, int action, int mods) override {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
            showDepthMap = !showDepthMap;
    }

    Model model;
    OpenGLShader light_shadow_shader, shadow_shader, debug_shader;
    OpenGLCamera camera;

    const GLsizei SHADOW_MAP_SIZE = 1024 * 2;

    GLuint shadowMapTexID, fboID;
    GLuint vao, index_buffer;
    GLuint buffer[2];

    bool showDepthMap = false;
};

int main() {
    Tutorial_OpenGL_Moderno win_app;

    if (win_app.init("OpenGL Moderno - Shadow Maps", 1280, 720)) {
        win_app.info();

        cout << "TOTURIALES OPENGL MODERNO - SHADOW MAP" << endl << endl;
        cout << "Presione la tecla [SPACE] para (mostrar/ocultar) el depth map." << endl;
        cout << "Usar la tecla [Esc] para salir." << endl << endl;

        win_app.run();
    }

    return 0;
}