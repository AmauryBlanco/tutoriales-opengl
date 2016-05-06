#ifndef OPENGLSHADER_H
#define OPENGLSHADER_H

#include <GL\glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

class OpenGLShader {

public:
    OpenGLShader() : program{}
    {}

    ~OpenGLShader()
    {
        glDeleteProgram(program);
    }

    GLuint getUniformLocation(std::string name) {
        return glGetUniformLocation(program, name.c_str());
    }

    bool loadFile(std::string filename, std::string& outcode)
    {
        std::ifstream file(filename, std::ios::in);

        if (!file)
        {
            std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
            return false;
        }

        file.seekg(0, std::ios::end);
        outcode.clear();
        outcode.reserve(static_cast<size_t>(file.tellg()));
        file.seekg(0, std::ios::beg);

        if (file.is_open()) {

            std::istreambuf_iterator<char> in{ file }; 
            std::istreambuf_iterator<char> end_in;

            while (in != end_in) outcode.push_back(*in++);

            file.close();
        }

        return true;
    }

    GLuint compile()
    {
        auto vertex_code = std::string(
            "#version 330 core\n"
            "in vec4 vPosition;"
            "out vec4 vs_color;"
            "void main(void)"
            "{"
            "gl_Position = vPosition;"
            "vs_color = vec4(0.0, 0.0, 0.0, 1.0);"
            "}");

        auto fragment_code = std::string(
            "#version 330 core\n"
            "in vec4 vs_color;"
            "out vec4 color;"
            "void main(void) { color = vs_color; }");

        GLuint vertex_shader = compileShader(vertex_code, GL_VERTEX_SHADER);
        GLuint fragment_shader = compileShader(fragment_code, GL_FRAGMENT_SHADER);

        return createProgram(vertex_shader, fragment_shader);
    }

    GLuint compile(std::string vertex_file, std::string fragment_file)
    {
        std::string vertex, fragment;

        if (loadFile(vertex_file, vertex) && loadFile(fragment_file, fragment)) {

            GLuint vertex_shader = compileShader(vertex, GL_VERTEX_SHADER);
            GLuint fragment_shader = compileShader(fragment, GL_FRAGMENT_SHADER);

            return createProgram(vertex_shader, fragment_shader);
        }

        return 0;
    }

    void logShader(GLuint shader, const GLchar* title)
    {
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if (status == GL_FALSE) {
            GLint len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

            auto log_ptr = std::make_unique<GLchar[]>(len + 1);
            glGetShaderInfoLog(shader, len, NULL, log_ptr.get());
            std::cerr << title << " shader compilation failed: " << std::endl << log_ptr.get() << std::endl;
        }
    }


    void logLink() {
        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (linked == GL_FALSE) {
            GLint len;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

            auto log_ptr = std::make_unique<GLchar[]>(len + 1);
            glGetProgramInfoLog(program, len, &len, log_ptr.get());
            std::cerr << "Shader linking failed: " << log_ptr.get() << std::endl;
        }
    }

    GLuint compileShader(std::string& source, GLenum type) {

        GLuint shader{ 0 };

        if (!source.empty()) {
            const GLchar* code = const_cast<GLchar*>(source.c_str());

            // crear el shader object
            shader = glCreateShader(type);
            // agregar el codigo al sahder
            glShaderSource(shader, 1, &code, NULL);
            // compilar el shader indicado
            glCompileShader(shader);

            logShader(shader, type == GL_FRAGMENT_SHADER ? "Fragment" : "Vertex");
        }

        return shader;
    }

    GLuint createProgram(GLuint vertex, GLuint fragment) {
        // crear el program object
        program = glCreateProgram();

        // añadir los shaders al programa
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);

        // enlazar el programa
        glLinkProgram(program);

        logLink();

        // eliminar los shaders, ya no son necesarios
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        return program;
    }

    void use()
    {
        glUseProgram(program);
    }

	void unUse() {
		glUseProgram(0);
	}

    GLuint getProgram() {
        return program;
    }

private:
    GLuint program;
};

#endif