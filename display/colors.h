#pragma once

#include <stdlib.h>
#include <cmath>

struct Color
{
    float r, g, b, a;
    const char* hex;

    bool operator!=(const Color& rhs) const
    {
        const float epsilon = 0.01f;
        return (fabs(r - rhs.r) > epsilon) ||
               (fabs(g - rhs.g) > epsilon) ||
               (fabs(b - rhs.b) > epsilon);
    }
};

struct RGB { float r, g, b, a; };

Color to_color(const char* hex, float a = 255)
{
    if (hex[0] == '#') hex++;

    Color color;
    color.hex = hex;

    char r[3] = { hex[0], hex[1], '\0' };
    char g[3] = { hex[2], hex[3], '\0' };
    char b[3] = { hex[4], hex[5], '\0' };

    color.r = strtol(r, nullptr, 16) / 255.0f;
    color.g = strtol(g, nullptr, 16) / 255.0f;
    color.b = strtol(b, nullptr, 16) / 255.0f;
    color.a = a / 255.0f;

    return color;
}

inline Color bgc = to_color("#279dff");
inline Color __fontcolor = to_color("#000000");
inline RGB bgcolor = { bgc.r, bgc.g, bgc.b, bgc.a };
#define BGCOLOR bgcolor
