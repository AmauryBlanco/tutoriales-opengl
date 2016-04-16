#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <algorithm>

using namespace std;

class OpenGLWindow {

public:
	OpenGLWindow() :
		aspect_ratio{},
		window{}
	{ }

	bool init(string title, int width = 1024, int height = 720, bool full_screen = false) {
		aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

		glfwSetErrorCallback(error_callback);

		if (!glfwInit()) return false;

		window = glfwCreateWindow(width, height, title.c_str(), full_screen ? glfwGetPrimaryMonitor() : NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			return false;
		}

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		glfwSetKeyCallback(window, key_callback);
		glfwSetWindowCloseCallback(window, close_callback);
		glfwSetFramebufferSizeCallback(window, resize_callback);
        glfwSetCursorPosCallback(window, mouse_callback);

		if (glewInit() != GLEW_OK) {
			glfwTerminate();
			return false;
		}

		glfwSetWindowUserPointer(window, this);

		return true;
	}

	void run() {
		onstart();
		glfwSetTime(0.0);

		while (!glfwWindowShouldClose(window))
		{
			double seconds = glfwGetTime();
			onrender(seconds);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void info() {
		cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
		cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
		cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
		cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
        cout << endl;
	}

    GLFWwindow* getGLFWwindow() {
        return window;
    }

private:

	static void error_callback(int error, const char* description)
	{
		cerr << "Error: " << error << ", " << description << endl << endl;
	}

	static void close_callback(GLFWwindow* window)
	{
		OpenGLWindow* win_app = getOpenGLWinApp(window);
		win_app->onstop();
	}

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		OpenGLWindow* win_app = getOpenGLWinApp(window);
		win_app->onkey(key, scancode, action, mods);
	}

	static void resize_callback(GLFWwindow* window, int width, int height)
	{
		OpenGLWindow* win_app = getOpenGLWinApp(window);
		win_app->onresize(width, height);
	}

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        OpenGLWindow* win_app = getOpenGLWinApp(window);
        win_app->onmouse(xpos, ypos);
    }

	inline static OpenGLWindow* getOpenGLWinApp(GLFWwindow* window) {
		return static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(window));
	}

protected:
	virtual void onstart() { };

	virtual void onstop() { };

	virtual void onkey(int key, int scancode, int action, int mods) { };

	virtual void onrender(double time) = 0;

    virtual void onmouse(double xpos, double ypos) { };
	
	virtual void onresize(int width, int height) {
		aspect_ratio = max(0.0f, width / (float)height);
		glViewport(0, 0, width, height);
	};

protected:
	GLFWwindow* window;
	float aspect_ratio;

};

#endif