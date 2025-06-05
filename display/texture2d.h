#pragma once

#include "bmp.h"

#define S(tilex, tiley, tilew, tileh, scalex, scaley) {{tilex, tiley, tilew, tileh}, {scalex, scaley}}

struct Texture2D
{
    BMP* image;
    GLTexture* data;
    int width, height;

    Texture2D(const char* path)
    {
        image = new BMP(path);
        data = new GLTexture(image);
        width = data->width;
        height = data->height;
    }

    Texture2D(BMP* bmp)
    {
        image = bmp;
        data = new GLTexture(image);
        width = data->width;
        height = data->height;
    }

    void refresh() { data->refresh(); }
};

struct Point { float x, y; };

struct Surface
{
    Rect data;
    Point scale;
};

#define IGNORE_OUTBOUNDS_XY(texture, x, y) \
    if ((x) + (texture)->width < 0 || (x) > SCREEN_WIDTH || (y) + (texture)->height < 0 || (y) > SCREEN_HEIGHT) \
        return;

#define IGNORE_OUTBOUNDS(texture, surface, x, y) \
    do { \
        float surf_w = (surface).data.width * (surface).scale.x; \
        float surf_h = (surface).data.height * (surface).scale.y; \
        if ((x) + surf_w < 0 || (x) > SCREEN_WIDTH || (y) + surf_h < 0 || (y) > SCREEN_HEIGHT) \
            return; \
    } while(0)
   
void blit(Texture2D* texture, float x, float y)
{
    IGNORE_OUTBOUNDS_XY(texture, x, y);

    glblit(texture->data, x, y);
}

void blit(Texture2D* texture, const Surface& surface, float x, float y)
{
    IGNORE_OUTBOUNDS(texture, surface, x, y);

    float vertices[24];
    memcpy(vertices, origin_vertices, sizeof(vertices));

    // Compute UV coordinates based on surface.rect and texture size
    float u0 = surface.data.x / (float)texture->width;
    float v0 = surface.data.y / (float)texture->height;
    float u1 = (surface.data.x + surface.data.width) / (float)texture->width;
    float v1 = (surface.data.y + surface.data.height) / (float)texture->height;

    // Replace UVs in vertices array
    for (int i = 0; i < 6; ++i)
    {
        int idx = i * 4;
        // UV coordinates are at idx + 2 and idx + 3
        vertices[idx + 2] = (vertices[idx + 2] == 0.0f) ? u0 : u1;
        vertices[idx + 3] = (vertices[idx + 3] == 0.0f) ? v0 : v1;
    }

    glBindBuffer(GL_ARRAY_BUFFER, texture->data->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glUseProgram(texture->data->shader_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->data->gl_index);
    glUniform1i(glGetUniformLocation(texture->data->shader_program, "tex"), 0);

    // Compute scaled quad size in screen coords
    float surf_w = surface.data.width * surface.scale.x;
    float surf_h = surface.data.height * surface.scale.y;

    float scale_x = surf_w / SCREEN_WIDTH;
    float scale_y = surf_h / SCREEN_HEIGHT;

    float offset_x = (2.0f * x / SCREEN_WIDTH) - 1.0f + scale_x;
    float offset_y = 1.0f - (2.0f * y / SCREEN_HEIGHT) - scale_y;

    glUniform2f(glGetUniformLocation(texture->data->shader_program, "scale"), scale_x, scale_y);
    glUniform2f(glGetUniformLocation(texture->data->shader_program, "offset"), offset_x, offset_y);

    glBindVertexArray(texture->data->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Restore original vertices to avoid side effects on next draw
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(origin_vertices), origin_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void blit(Texture2D* texture, float x, float y, float scalex, float scaley = -1)
{
    if(scaley == -1) scaley = scalex;

    int width = texture->image->width;
    int height = texture->image->height;

    blit(texture, S(0, 0, width, height, scalex, scaley), x, y);
}