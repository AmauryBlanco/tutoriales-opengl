#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"

#include <fstream>

using namespace std;

class Tutorial_04_A : public OpenGLWindow {

public:
	Tutorial_04_A() : vao{}, buffer{}, textureID{}, secondTextureID{}, programID{} { }

private:
	void onstart() override {

		programID = shader_simple.compile("shaders/simple.vertex_shader", "shaders/simple.fragment_shader");

		// crear y enlazar un vao
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		static const float vertex[] =
		{
			-1.0f,  1.0f, 0.0f, 1.5f,
			 1.0f,  1.0f, 0.0f, 1.5f,
			-1.0f, -1.0f, 0.0f, 1.5f,
			 1.0f, -1.0f, 0.0f, 1.5f
		};

		static const float uv[] =
		{
			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f
		};

		textureID = loadBMP("textures/lena.bmp");
		secondTextureID = loadBMP("textures/baboon.bmp");

		// generar dos ids para los buffer
		glGenBuffers(2, buffer);

		// enlazar el buffer de vertices
		glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
		// almacenar datos en el buffer de vertices
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

		// describir los datos y activar vPosition (location = 0)
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		// crear en buffer de coordenadas UV
		glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
	}

	void onrender(double time) override {
		glClear(GL_COLOR_BUFFER_BIT);
		shader_simple.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(glGetUniformLocation(programID, "myTexture"), 0);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, secondTextureID);
		glUniform1i(glGetUniformLocation(programID, "secondTexture"), 1);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void onstop() override {
		glDeleteBuffers(2, buffer);
		glDeleteTextures(1, &textureID);
		glDeleteTextures(1, &secondTextureID);
		glDeleteVertexArrays(1, &vao);
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
			GLuint textureID;
			glGenTextures(1, &textureID);

			// Enlazar la textura recien creada
			// GL_TEXTURE_2D la textura es un arreglo de pixeles de 2 dimensiones
			glBindTexture(GL_TEXTURE_2D, textureID);

			// Crear la textura
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

			// Liberar memoria usada por la imagen BMP
			delete[] data;
			 
			// filtering, wrapping and mip maps
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			return textureID;
		}

		return 0;
	}

	OpenGLShader shader_simple;
	GLuint vao, textureID, secondTextureID, programID;
	GLuint buffer[2];
};

int main() {
	Tutorial_04_A win_app;

	if (win_app.init("Tutoriales OpenGL Moderno - Texturas")) {
		win_app.info();
		win_app.run();
	}
	return 0;
}