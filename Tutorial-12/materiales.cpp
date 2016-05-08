#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLCamera.hpp"

#include "ModelMesh.cpp"

#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <string>

class Tutorial_07 : public OpenGLWindow {

public:
    Tutorial_07() :
        luz_direction{ 0, 10, 10 } { }

private:
    void onstart() override {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.5f, 0.5f, 1.0f);

        camera.setWindow(this->window);
        dir_shader.compile("shaders/directional.vertex_shader", "shaders/directional.fragment_shader");
        model.init("model/test.assbin");

        // ocultar el cursor y ubicarlo en el centro de la ventana
        glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(this->window, 1280 / 2, 720 / 2);
    }

    void onrender(double time) override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        luzDirectional(dir_shader, time);
    }

    void luzDirectional(OpenGLShader& shader, double time) {
        shader.use();

        glm::mat4 Model;
 
        glm::mat4 MV  = camera.getViewMatrix() * Model;
        glm::mat4 MVP = camera.getProjectionMatrix() * MV;
        glm::mat3 N   = glm::inverseTranspose(glm::mat3(MV));

        glUniformMatrix4fv(shader.getUniformLocation("mvp_matrix"), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniformMatrix4fv(shader.getUniformLocation("mv_matrix"), 1, GL_FALSE, glm::value_ptr(MV));
        glUniformMatrix3fv(shader.getUniformLocation("n_matrix"), 1, GL_FALSE, glm::value_ptr(N));

        // direccion de la luz
        glm::vec3 lightDirEyeSpace = glm::vec3(MV * glm::vec4(luz_direction, 0));

        glUniform3fv(shader.getUniformLocation("light.direction"), 1, glm::value_ptr(lightDirEyeSpace));
        glUniform3fv(shader.getUniformLocation("light.ambient"), 1, glm::value_ptr(glm::vec3(0.1f)));
        glUniform3fv(shader.getUniformLocation("light.diffuse"), 1, glm::value_ptr(glm::vec3(1.0f)));
        glUniform3fv(shader.getUniformLocation("light.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));

        model.draw(shader.getProgram());

        shader.unUse();
    }

    void onkey(int key, int scancode, int action, int mods) override {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (key == GLFW_KEY_LEFT)
            luz_direction.x += 0.1f;
        if (key == GLFW_KEY_RIGHT)
            luz_direction.x -= 0.1f;
        if (key == GLFW_KEY_UP)
            luz_direction.y += 0.1f;
        if (key == GLFW_KEY_DOWN)
            luz_direction.y -= 0.1f;
    }

    Model model;
    OpenGLShader dir_shader; 
    OpenGLCamera camera;

    glm::vec3 luz_direction;
};

int main() {
    Tutorial_07 win_app;

    if (win_app.init("OpenGL Moderno - Assimp Materiales", 1280, 720)) {
        win_app.info();
        win_app.run();
    }

    return 0;
}