#pragma once

#include <GLFW/glfw3.h>
#include <GL/glcorearb.h>
#include <GL/glext.h>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "colors.h"
#include "rect.h"

#define MAX_BMP_W 512
#define MAX_BMP_H 512
#define MAX_PALETTE 256

struct BMP
{
    int width, height;
    uint8_t map[MAX_BMP_W * MAX_BMP_H];
    Color palette[MAX_PALETTE];
    int palette_size = 0;
    uint8_t data[MAX_BMP_W * MAX_BMP_H * 4];

    BMP(const char* path)
    {
        FILE* f = fopen((string("assets/") + path).c_str(), "rb");

        if (!f)
        {
            printf("Failed to open BMP file: %s\n", path);
            return;
        }

        uint8_t header[54];
        fread(header, 1, 54, f);

        width = *(int*)&header[18];
        height = *(int*)&header[22];
        int offset = *(int*)&header[10];
        int bpp = *(short*)&header[28];

        if (bpp != 24) { fclose(f); return; }

        fseek(f, offset, SEEK_SET);
        int row_padded = (width * 3 + 3) & (~3);
        uint8_t* row = new uint8_t[row_padded];

        palette_size = 0;

        for (int y = 0; y < height; ++y) {
            fread(row, 1, row_padded, f);
            for (int x = 0; x < width; ++x) {
                uint8_t b = row[x * 3 + 0];
                uint8_t g = row[x * 3 + 1];
                uint8_t r = row[x * 3 + 2];

                Color c = { r / 255.0f, g / 255.0f, b / 255.0f, 1.0f, nullptr };

                int index = -1;
                for (int i = 0; i < palette_size; ++i) 
                {
                    Color p = palette[i];
                    if (p.r == c.r && p.g == c.g && p.b == c.b && p.a == c.a) {
                        index = i;
                        break;
                    }
                }

                if (index == -1) {
                    if (palette_size >= MAX_PALETTE)
                    {
                        delete[] row;
                        fclose(f);
                        return;
                    }
                    index = palette_size;
                    palette[palette_size++] = c;
                }

                int i = (height - 1 - y) * width + x;
                map[i] = index;
            }
        }

        delete[] row;
        fclose(f);

        refresh();
    }

    void refresh()
    {
        for (int i = 0; i < width * height; ++i)
        {
            Color c = palette[map[i]];
            int o = i * 4;
            data[o + 0] = (uint8_t)(c.r * 255);
            data[o + 1] = (uint8_t)(c.g * 255);
            data[o + 2] = (uint8_t)(c.b * 255);

            data[o + 3] = (map[i] == 0) ? 0 : (uint8_t)(c.a * 255);
        }
    }

    void set(int i, int j, const Color& color)
    {
        if (i < 0 || i >= width || j < 0 || j >= height) return;

        int index = -1;
        for (int k = 0; k < palette_size; ++k) {
            Color& p = palette[k];
            if (p.r == color.r && p.g == color.g && p.b == color.b && p.a == color.a) {
                index = k;
                break;
            }
        }

        if (index == -1)
        {
            if (palette_size >= MAX_PALETTE) return;
            index = palette_size++;
            palette[index] = color;
        }

        int pos = j * width + i;
        map[pos] = (uint8_t)index;

        int o = pos * 4;
        data[o + 0] = (uint8_t)(color.r * 255);
        data[o + 1] = (uint8_t)(color.g * 255);
        data[o + 2] = (uint8_t)(color.b * 255);
        data[o + 3] = (uint8_t)(color.a * 255);
    }

    void set(int i, int j, float r, float g, float b, float a = 1.0f)
    {
        Color color = { r, g, b, a, nullptr };
        set(i, j, color);
    }
};

static GLuint compile_shader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        printf("Shader compile error: %s\n", infoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint create_shader_program(const char* vertex_source, const char* fragment_source)
{
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    if (!vertex_shader || !fragment_shader) return 0;

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        printf("Shader program link error: %s\n", infoLog);
        glDeleteProgram(program);
        return 0;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

extern const float origin_vertices[24] = {
    -1.0f,  1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 1.0f,

    -1.0f,  1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        1.0f,  1.0f, 1.0f, 0.0f
};

struct GLTexture
{
    int width, height;
    GLuint gl_index;

    GLuint VAO, VBO;
    GLuint shader_program;

    const BMP* _bmp_ref;

    const char* vertex_shader_src = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aTexCoord;
        out vec2 vTexCoord;
        uniform vec2 scale;
        uniform vec2 offset;
        void main()
        {
            vec2 pos = aPos * scale + offset;
            gl_Position = vec4(pos, 0.0, 1.0);
            vTexCoord = aTexCoord;
        }
    )";

    const char* fragment_shader_src = R"(
        #version 330 core
        in vec2 vTexCoord;
        out vec4 FragColor;
        uniform sampler2D tex;
        void main()
        {
            FragColor = texture(tex, vTexCoord);
        }
    )";
 
    GLTexture(const BMP* bmp)
    {
        width = bmp->width;
        height = bmp->height;

        glGenTextures(1, &gl_index);
        glBindTexture(GL_TEXTURE_2D, gl_index);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp->data);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(origin_vertices), origin_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        shader_program = create_shader_program(vertex_shader_src, fragment_shader_src);

        _bmp_ref = bmp;
    }

    void refresh()
    {
        glBindTexture(GL_TEXTURE_2D, gl_index);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, _bmp_ref->data);
    }
};

void glblit(GLTexture* texture, float x, float y)
{
    glUseProgram(texture->shader_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->gl_index);
    glUniform1i(glGetUniformLocation(texture->shader_program, "tex"), 0);

    float scale_x = (float)texture->width / SCREEN_WIDTH;
    float scale_y = (float)texture->height / SCREEN_HEIGHT;
    float offset_x = (2.0f * x / SCREEN_WIDTH) - 1.0f + scale_x;
    float offset_y = 1.0f - (2.0f * y / SCREEN_HEIGHT) - scale_y;

    GLint scale_loc = glGetUniformLocation(texture->shader_program, "scale");
    GLint offset_loc = glGetUniformLocation(texture->shader_program, "offset");

    glUniform2f(scale_loc, scale_x, scale_y);
    glUniform2f(offset_loc, offset_x, offset_y);

    glBindVertexArray(texture->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
