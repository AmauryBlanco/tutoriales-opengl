#ifndef OPENGLTEXT_H
#define OPENGLTEXT_H

#include <cstdio>
#include <string>
#include <vector>

#include <GL\GL.h>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "OpenGLShader.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb\stb_rect_pack.h"
#include "stb\stb_truetype.h"
#include "stb\stb_image_write.h"

#define BITMAP_W 1024
#define BITMAP_H 1024

class OpenGLText {

private:
    GLuint font_tex, vao, buffer, ortho_loc, text_loc, color_loc;
    stbtt_packedchar chardata[256];
    OpenGLShader shader;
    float width, height;
    float r, g, b;

    void init() {

        GLuint program = shader.compile("shaders/text.vertex_shader", "shaders/text.fragment_shader");

        ortho_loc = glGetUniformLocation(program, "projection");
        text_loc = glGetUniformLocation(program, "font");
        color_loc = glGetUniformLocation(program, "font_color");

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

        glBindVertexArray(0);
    }

public:
    void loadFont(const std::string& font, float font_size)
    {
        FILE *file;
        if(fopen_s(&file, font.c_str(), "rb")){
            std::cout << "No encuentra el archivo: " << font << std::endl;
            return;
        }

        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        unsigned char* ttf_buffer = new unsigned char[size];
        unsigned char* temp_bitmap = new unsigned char[BITMAP_W * BITMAP_H];

        fread(ttf_buffer, 1, size, file);

        stbtt_pack_context pc; 

        stbtt_PackBegin(&pc, temp_bitmap, BITMAP_W, BITMAP_H, 0, 1, NULL);
        stbtt_PackSetOversampling(&pc, 2, 2);
        stbtt_PackFontRange(&pc, ttf_buffer, 0, font_size, 0, 256, chardata);
        stbtt_PackEnd(&pc);

        //stbi_write_png("font.png", BITMAP_W, BITMAP_H, 1, temp_bitmap, 0);

        glGenTextures(1, &font_tex);
        glBindTexture(GL_TEXTURE_2D, font_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, BITMAP_W, BITMAP_H, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        delete[] ttf_buffer;
        delete[] temp_bitmap;

        fclose(file);
        file = NULL;

        init();
    }

    void draw(float x, float y, const std::wstring& text) {
        shader.use();
        glBindVertexArray(vao);

        glUniform3f(color_loc, r, g, b);

        glm::mat4 ortho_proj = glm::ortho(0.0f, width, height, 0.0f);
        glUniformMatrix4fv(ortho_loc, 1, GL_FALSE, &ortho_proj[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font_tex);
        glUniform1i(text_loc, 0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        std::wstring::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)        {
            stbtt_aligned_quad q;
            stbtt_GetPackedQuad(chardata, BITMAP_W, BITMAP_H, *c, &x, &y, &q, 1);

            const GLfloat vertex[] = {
                q.s0, q.t0, q.x0, q.y0,
                q.s1, q.t0, q.x1, q.y0,
                q.s1, q.t1, q.x1, q.y1,

                q.s0, q.t0, q.x0, q.y0,
                q.s1, q.t1, q.x1, q.y1,
                q.s0, q.t1, q.x0, q.y1
            };

            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glEnableVertexAttribArray(0);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glDisable(GL_BLEND);
        glBindVertexArray(0);
        shader.unUse();
    }

    void setSize(int width, int height) {
        this->width = static_cast<float>(width);
        this->height = static_cast<float>(height);
    }

    void setColor(float r, float g, float b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
};

#endif