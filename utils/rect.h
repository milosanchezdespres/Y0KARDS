#pragma once

#include <string.h>

struct Rect
{
    float x, y;
    int width, height;
};

void rect_to_vertices(const Rect& rect, float screenW, float screenH, float vertices[24])
{
    float x_ndc = (rect.x / screenW) * 2.f - 1.f;
    float y_ndc = 1.f - (rect.y / screenH) * 2.f;
    float w_ndc = (rect.width / (float)screenW) * 2.f;
    float h_ndc = (rect.height / (float)screenH) * 2.f;

    float v[] = {
        x_ndc,         y_ndc,          0.f, 1.f,
        x_ndc,         y_ndc - h_ndc,  0.f, 0.f,
        x_ndc + w_ndc, y_ndc - h_ndc,  1.f, 0.f,

        x_ndc,         y_ndc,          0.f, 1.f,
        x_ndc + w_ndc, y_ndc - h_ndc,  1.f, 0.f,
        x_ndc + w_ndc, y_ndc,          1.f, 1.f
    };

    memcpy(vertices, v, sizeof(v));
}

Rect vertices_to_rect(const float vertices[24], float screenW, float screenH)
{
    float x_ndc = vertices[0];
    float y_ndc = vertices[1];
    float x2_ndc = vertices[8];
    float y2_ndc = vertices[9];

    float w_ndc = x2_ndc - x_ndc;
    float h_ndc = y_ndc - y2_ndc;

    Rect rect;
    rect.x = ((x_ndc + 1.f) / 2.f) * screenW;
    rect.y = ((1.f - y_ndc) / 2.f) * screenH;
    rect.width = static_cast<int>(w_ndc * screenW / 2.f * 2.f);  // w_ndc * screenW simplified
    rect.height = static_cast<int>(h_ndc * screenH / 2.f * 2.f); // h_ndc * screenH simplified

    return rect;
}
