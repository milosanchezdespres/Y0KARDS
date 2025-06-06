#pragma once

#include <stdlib.h>
#include <cmath>

struct RGB { float r, g, b, a; };

string to_hex(float R, float G, float B, float A)
{
    // Clamp and convert to 0-255 integer range
    int r = static_cast<int>(R * 255.0f);
    int g = static_cast<int>(G * 255.0f);
    int b = static_cast<int>(B * 255.0f);

    // Clamp values just in case
    if (r < 0) r = 0; if (r > 255) r = 255;
    if (g < 0) g = 0; if (g > 255) g = 255;
    if (b < 0) b = 0; if (b > 255) b = 255;

    char hex[8];  // # + 6 hex digits + \0
    snprintf(hex, sizeof(hex), "#%02X%02X%02X", r, g, b);

    return string(hex);
}

RGB to_rgb(const char* hex, float A = 255)
{
    if (hex[0] == '#') hex++;

    char R[3] = { hex[0], hex[1], '\0' };
    char G[3] = { hex[2], hex[3], '\0' };
    char B[3] = { hex[4], hex[5], '\0' };

    float r = strtol(R, nullptr, 16) / 255.0f;
    float g = strtol(G, nullptr, 16) / 255.0f;
    float b = strtol(B, nullptr, 16) / 255.0f;
    float a = A / 255.0f;

    return {r, g, b, a};
}

struct Color
{
    float r, g, b, a;
    const char* hex;

    Color() {}

    Color(float R, float G, float B, float A = 255)
    {
        r = R;
        g = G;
        b = B;
        a = A;

        hex = to_hex(r, g, b, a).c_str();
    }

    Color(const char* HEX)
    {
        hex = HEX;
        RGB rgb = to_rgb(hex);
        r = rgb.r;
        g = rgb.g;
        b = rgb.b;
        a = rgb.a;
    }

    bool operator!=(const Color& rhs) const
    {
        const float epsilon = 0.01f;
        return (fabs(r - rhs.r) > epsilon) ||
               (fabs(g - rhs.g) > epsilon) ||
               (fabs(b - rhs.b) > epsilon);
    }
};

Color to_color(const char* hex, float a = 255)
{
    if (hex[0] == '#') hex++;

    Color color;
    color.hex = hex;
    
    RGB rgb = to_rgb(hex);
    color.r = rgb.r;
    color.g = rgb.g;
    color.b = rgb.b;
    color.a = rgb.a;

    return color;
}

inline Color bgc = to_color("#279dff");
inline Color __fontcolor = to_color("#000000");
inline RGB bgcolor = { bgc.r, bgc.g, bgc.b, bgc.a };
#define BGCOLOR bgcolor
