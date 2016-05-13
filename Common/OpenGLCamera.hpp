#ifndef OPENGLCAMERA_H
#define OPENGLCAMERA_H

#include <GL\GL.h>

#include <glm\glm.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\matrix_cross_product.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <GLFW\glfw3.h>

class OpenGLCamera {
public:
    OpenGLCamera() : yaw{ 0 }, pitch{ 0 },
        cameraPos{ 0.0f, 4.5f, 15.0f },
        cameraFront{ 0.0f, 0.0f, -1.0f },
        cameraUp{ 0.0f, 1.0f, 0.0f },
        cameraSpeed{ 0.05f }, mouseSensitive{ 0.15f } {
    }

private:   
    float yaw, pitch;
    glm::vec3 cameraPos, cameraFront, cameraUp;
    GLfloat cameraSpeed, mouseSensitive;
    GLFWwindow* window;

    void checkMouse() {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetCursorPos(window, width / 2.0, height / 2.0);

        GLfloat xoffset = ((width / 2.0f) - static_cast<float>(xpos)) * mouseSensitive;
        GLfloat yoffset = ((height / 2.0f) - static_cast<float>(ypos)) * mouseSensitive;

        yaw += xoffset;
        pitch += yoffset;
    }

    void chechKeys() {
        if (isKeyPress(GLFW_KEY_W))
            cameraPos += cameraSpeed * cameraFront;
        if (isKeyPress(GLFW_KEY_S))
            cameraPos -= cameraSpeed * cameraFront;
        if (isKeyPress(GLFW_KEY_A))
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (isKeyPress(GLFW_KEY_D))
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    inline bool isKeyPress(int key) {
        return (glfwGetKey(window, key) == GLFW_PRESS);
    }

public:
    void setWindow(GLFWwindow* window) {
        this->window = window;
    }

    glm::mat4 getViewMatrix() {
        chechKeys();
        checkMouse();

        // obtener los parametros de visualizacion
        glm::mat4 Rot = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
        cameraFront = glm::vec3(Rot * glm::vec4(0, 0, -1, 0));
        cameraUp = glm::vec3(Rot * glm::vec4(0, 1, 0, 0));

        // craer la matriz de vista
        return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    glm::mat4 getProjectionMatrix() {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        return glm::perspective(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
    }

    glm::vec3 getPosition() {
        return cameraPos;
    }
};

#endif