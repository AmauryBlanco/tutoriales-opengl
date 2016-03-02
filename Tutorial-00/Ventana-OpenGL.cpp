#include "OpenGLWindow.hpp"

class Tutorial_00 : public OpenGLWindow {
	void onrender(double time) override {
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-aspect_ratio, aspect_ratio, -1.f, 1.f, 1.f, -1.f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(static_cast<GLfloat>(time) * 50.f, 0.f, 0.f, 1.f);

		glBegin(GL_TRIANGLES);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(-0.6f, -0.4f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(0.6f, -0.4f, 0.f);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.6f, 0.f);
		glEnd();
	}
};

int main(void) {
	Tutorial_00 win_app;
	if (win_app.init("Tutoriales OpenGL Moderno")) {
		win_app.info();
		win_app.run();
	}
	return 0;
}

