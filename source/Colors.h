#pragma once

#include <array>
#include <string.h>
#include <iostream>
using namespace std;

struct RGBA
{
    float r, g, b, a;

    RGBA(float R, float G, float B, float A = 255)
    {
        r = R / 255.0f;
        g = G / 255.0f;
        b = B / 255.0f;
        a = A / 255.0f;
    }

    string str() const
    {
        char RR[3], GG[3], BB[3], AA[3];
        char buffer[10] = {};

        __int_to_hex2(static_cast<int>(r * 255), RR);
        __int_to_hex2(static_cast<int>(g * 255), GG);
        __int_to_hex2(static_cast<int>(b * 255), BB);
        __int_to_hex2(static_cast<int>(a * 255), AA);

        buffer[0] = '#';
        buffer[1] = AA[0];
        buffer[2] = AA[1];
        buffer[3] = RR[0];
        buffer[4] = RR[1];
        buffer[5] = GG[0];
        buffer[6] = GG[1];
        buffer[7] = BB[0];
        buffer[8] = BB[1];
        buffer[9] = '\0';

        return string(buffer);
    }

    void __int_to_hex2(int val, char out[3]) const { snprintf(out, 3, "%02X", val & 0xFF); }
};

struct HEX
{
    char RR[3], GG[3], BB[3], AA[3];

    string str() const
    {
        char buffer[10] = {};

        buffer[0] = '#';
        buffer[1] = RR[0];
        buffer[2] = RR[1];
        buffer[3] = GG[0];
        buffer[4] = GG[1];
        buffer[5] = BB[0];
        buffer[6] = BB[1];
        buffer[7] = AA[0];
        buffer[8] = AA[1];
        buffer[9] = '\0';

        return string(buffer);
    }

    HEX(const char* _value)
    {
        const char* p = (_value[0] == '#') ? _value + 1 : _value;

        memcpy(RR, p, 2); RR[2] = '\0';
        memcpy(GG, p + 2, 2); GG[2] = '\0';
        memcpy(BB, p + 4, 2); BB[2] = '\0';

        AA[0] = '\0';
        if (strlen(p) >= 8) memcpy(AA, p + 6, 2); AA[2] = '\0';
    }

    int r() const { return __hex_to_int(RR); }
    int g() const { return __hex_to_int(GG); }
    int b() const { return __hex_to_int(BB); }
    int a() const { return AA[0] ? __hex_to_int(AA) : 255; } 

    int __hex_to_int(const char* hex) const
    {
        int value = 0;
        sscanf(hex, "%2x", &value);
        return value;
    }
};

inline RGBA to_rgba(const HEX& hex) { return RGBA(hex.r(), hex.g(), hex.b(), hex.a()); }
inline HEX to_hex(const RGBA& rgba) { return HEX(rgba.str().c_str()); }

namespace std
{
    inline string to_string(const HEX& hex) { return hex.str(); }
    inline string to_string(const RGBA& rgba) { return rgba.str(); }

    inline ostream& operator<<(ostream& os, const RGBA& color) { return os << color.str(); }
    inline ostream& operator<<(ostream& os, const HEX& hex) { return os << hex.str(); }
}

#define DARK_WHITE    RGBA{215, 215, 215}
#define WHITE         RGBA{255, 255, 255}
#define LIGHT_WHITE   RGBA{255, 255, 255}

#define DARK_BLACK    RGBA{  0,   0,   0}
#define BLACK         RGBA{  0,   0,   0}
#define LIGHT_BLACK   RGBA{ 40,  40,  40}

#define DARK_RED      RGBA{215,   0,   0}
#define RED           RGBA{255,   0,   0}
#define LIGHT_RED     RGBA{255,  40,  40}

#define DARK_GREEN    RGBA{  0, 215,   0}
#define GREEN         RGBA{  0, 255,   0}
#define LIGHT_GREEN   RGBA{ 40, 255,  40}

#define DARK_BLUE     RGBA{  0,   0, 215}
#define BLUE          RGBA{  0,   0, 255}
#define LIGHT_BLUE    RGBA{ 40,  40, 255}

#define DARK_YELLOW   RGBA{215, 215,   0}
#define YELLOW        RGBA{255, 255,   0}
#define LIGHT_YELLOW  RGBA{255, 255,  40}

#define DARK_CYAN     RGBA{  0, 215, 215}
#define CYAN          RGBA{  0, 255, 255}
#define LIGHT_CYAN    RGBA{ 40, 255, 255}

#define DARK_MAGENTA  RGBA{215,   0, 215}
#define MAGENTA       RGBA{255,   0, 255}
#define LIGHT_MAGENTA RGBA{255,  40, 255}

#define DARK_GRAY     RGBA{ 88,  88,  88}
#define GRAY          RGBA{128, 128, 128}
#define LIGHT_GRAY    RGBA{168, 168, 168}

#define DARK_ORANGE   RGBA{215, 125,   0}
#define ORANGE        RGBA{255, 165,   0}
#define LIGHT_ORANGE  RGBA{255, 205,  40}

#define DARK_PURPLE   RGBA{ 88,   0,  88}
#define PURPLE        RGBA{128,   0, 128}
#define LIGHT_PURPLE  RGBA{168,  40, 168}

#define DARK_BROWN    RGBA{125,  42,  42}
#define BROWN         RGBA{165,  42,  42}
#define LIGHT_BROWN   RGBA{205,  82,  82}
