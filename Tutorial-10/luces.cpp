#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLCamera.hpp"
#include "OpenGLModel.hpp"

#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <string>

class Tutorial_07 : public OpenGLWindow {

public:
    Tutorial_07() :
        luz_position{ 0, 4.5, 0 },
        luz_direction{ 0, 4, 3 },
        spot_cutoff{ 0.8f },
        spot_exponent{ 0.1f },
        luz{ 'b' },
        luz_vao{ 0 } { }

private:
    void onstart() override {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.25f, 0.80f, 0.75f, 1.0f);

        camera.setWindow(this->window);

        dir_shader.compile("shaders/directional.vertex_shader", "shaders/directional.fragment_shader");
        spot_shader.compile("shaders/spot.vertex_shader", "shaders/spot.fragment_shader");
        point_shader.compile("shaders/point.vertex_shader", "shaders/point.fragment_shader");

        model.init("model/scene.assbin");

        createLightPosition();
    }

    void onrender(double time) override {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        switch (luz)
        {
        case 'c': luzDirectional(dir_shader, time); break;
        case 'b': luzPuntual(point_shader, time); break;
        case 'a': luzFocal(spot_shader, time); break;
        }
    }

    void luzDirectional(OpenGLShader& shader, double time) {
        shader.use();

        glm::mat4 Model;
        glm::mat4 MV = camera.getViewMatrix() * Model;
        glm::mat4 MVP = camera.getProjectionMatrix() * MV;
        glm::mat3 N = glm::inverseTranspose(glm::mat3(MV));

        glUniformMatrix4fv(shader.getUniformLocation("mvp_matrix"), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniformMatrix4fv(shader.getUniformLocation("mv_matrix"), 1, GL_FALSE, glm::value_ptr(MV));
        glUniformMatrix3fv(shader.getUniformLocation("n_matrix"), 1, GL_FALSE, glm::value_ptr(N));

        // direccion de la luz
        glm::vec3 lightDir = glm::vec3(MV * glm::vec4(luz_direction, 0));
        glUniform3fv(shader.getUniformLocation("light_direction"), 1, glm::value_ptr(lightDir));

        model.draw();
        shader.unUse();
    }

    void luzFocal(OpenGLShader& shader, double time) {
        shader.use();
        updateLight(shader, time);

        // matrices 
        glm::mat4 Model;
        glm::mat4 MV = camera.getViewMatrix() * Model;
        glm::mat4 MVP = camera.getProjectionMatrix() * MV;
        glm::mat3 N = glm::inverseTranspose(glm::mat3(MV));

        glUniformMatrix4fv(shader.getUniformLocation("mvp_matrix"), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniformMatrix4fv(shader.getUniformLocation("mv_matrix"), 1, GL_FALSE, glm::value_ptr(MV));
        glUniformMatrix3fv(shader.getUniformLocation("n_matrix"), 1, GL_FALSE, glm::value_ptr(N));

        glm::vec3 spotPos(luz_position.x, 0, luz_position.z);
        glm::vec3 spotDir = glm::normalize(glm::vec3(MV * glm::vec4(spotPos - luz_position, 0)));

        glUniform3fv(shader.getUniformLocation("spot_direction"), 1, glm::value_ptr(spotDir));
        glUniform1f(shader.getUniformLocation("spot_exponent"), spot_exponent);
        glUniform1f(shader.getUniformLocation("spot_cutoff"), spot_cutoff);

        model.draw();
        shader.unUse();
    }

    void luzPuntual(OpenGLShader& shader, double time) {
        shader.use();
        updateLight(shader, time);

        // matrices 
        glm::mat4 Model;
        glm::mat4 MV = camera.getViewMatrix() * Model;
        glm::mat4 MVP = camera.getProjectionMatrix() * MV;
        glm::mat3 N = glm::inverseTranspose(glm::mat3(MV));

        glUniformMatrix4fv(shader.getUniformLocation("mvp_matrix"), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniformMatrix4fv(shader.getUniformLocation("mv_matrix"), 1, GL_FALSE, glm::value_ptr(MV));
        glUniformMatrix3fv(shader.getUniformLocation("n_matrix"), 1, GL_FALSE, glm::value_ptr(N));

        model.draw();
        shader.unUse();
    }

    void updateLight(OpenGLShader& shader, double time) {
        updateLightPosition(shader.getUniformLocation("light_pos"), static_cast<float>(time));
        drawLightPosition(shader);
    }

    void updateLightPosition(GLuint pos_loc, float time) {
        GLfloat radius = 2.5f;

        luz_position.x = sin(time) * radius;
        luz_position.z = cos(time) * radius;

        glUniform3fv(pos_loc, 1, glm::value_ptr(luz_position));
    }

    void drawLightPosition(OpenGLShader& shader) {
        glm::mat4 Model;
        Model = glm::translate(Model, luz_position);

        glm::mat4 MV = camera.getViewMatrix() * Model;
        glm::mat4 MVP = camera.getProjectionMatrix() * MV;
        glm::mat3 N = glm::inverseTranspose(glm::mat3(MV));

        glUniformMatrix4fv(shader.getUniformLocation("mvp_matrix"), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniformMatrix4fv(shader.getUniformLocation("mv_matrix"), 1, GL_FALSE, glm::value_ptr(MV));
        glUniformMatrix3fv(shader.getUniformLocation("n_matrix"), 1, GL_FALSE, glm::value_ptr(N));

        glBindVertexArray(luz_vao);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);
    }

    void createLightPosition() {
        glGenVertexArrays(1, &luz_vao);
        glBindVertexArray(luz_vao);

        static const float vertex[] =
        {
            1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f,
        };

        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void onkey(int key, int scancode, int action, int mods) override {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (key == GLFW_KEY_C)
            spot_cutoff += 0.01f;
        if (key == GLFW_KEY_V)
            spot_cutoff -= 0.01f;
        if (key == GLFW_KEY_F)
            spot_exponent += 0.1f;
        if (key == GLFW_KEY_G)
            spot_exponent -= 0.1f;

        if (key == GLFW_KEY_LEFT)
            luz_direction.x += 0.1f;
        if (key == GLFW_KEY_RIGHT)
            luz_direction.x -= 0.1f;

        if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
            luz = (luz == 'c') ? 'a' : luz + 1;
    }

    Model model;
    OpenGLShader point_shader, spot_shader, dir_shader;
    OpenGLCamera camera;

    GLuint luz_vao;
    glm::vec3 luz_position, luz_direction;

    float spot_cutoff, spot_exponent;
    char luz;
};

int main() {
    Tutorial_07 win_app;

    if (win_app.init("OpenGL Moderno - Luces", 1280, 720)) {

        // ocultar el cursor y ubicarlo en el centro de la ventana
        glfwSetInputMode(win_app.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(win_app.getGLFWwindow(), 1280 / 2, 720 / 2);

        win_app.info();
        win_app.run();
    }

    return 0;
}