#pragma once

#include <unordered_map>
#include <cctype>
#include <cstdio>
using namespace std;

#include "assets.h"

inline const Surface& letter(const char& l, int scale = 1)
{
    constexpr int LETTER_WIDTH = 16;
    constexpr int LETTER_HEIGHT = 16;
    constexpr int IMAGE_COLS = 8;

    static const std::pair<char, int> char_map_array[] = {
        {'A', 0}, {'B', 1}, {'C', 2}, {'D', 3}, {'E', 4}, {'F', 5}, {'G', 6}, {'H', 7},
        {'I', 8}, {'J', 9}, {'K',10}, {'L',11}, {'M',12}, {'N',13}, {'O',14}, {'P',15},
        {'Q',16}, {'R',17}, {'S',18}, {'T',19}, {'U',20}, {'V',21}, {'W',22}, {'X',23},
        {'Y',24}, {'Z',25}, {'0',26}, {'1',27}, {'2',28}, {'3',29}, {'4',30}, {'5',31},
        {'6',32}, {'7',33}, {'8',34}, {'9',35}, {'.',36}, {',',37}, {':',38}, {'!',39}, {'?',40}
    };

    static const unordered_map<char, int> char_index(
        char_map_array, 
        char_map_array + sizeof(char_map_array) / sizeof(char_map_array[0])
    );

    constexpr int CHARSET_SIZE = sizeof(char_map_array) / sizeof(char_map_array[0]);

    static const Surface fontSurface = { {0, 0, 128, 144}, {scale, scale} };
    static Surface cache[CHARSET_SIZE] = {};
    static bool initialized[CHARSET_SIZE] = {};

    auto it = char_index.find(l);
    if (it == char_index.end())
    {
        static Surface missing = { {0, 0, LETTER_WIDTH, LETTER_HEIGHT}, {scale, scale} };
        return missing;
    }

    int index = it->second;

    if (!initialized[index])
    {
        int col = index % IMAGE_COLS;
        int row = index / IMAGE_COLS;

        cache[index] = {
            { float(col * LETTER_WIDTH), float(row * LETTER_HEIGHT), LETTER_WIDTH, LETTER_HEIGHT },
            fontSurface.scale
        };
        initialized[index] = true;
    }

    return cache[index];
}

#define C(l, scale) letter(l, scale)

inline bool _font_init = false;
inline Texture2D* font = nullptr;

inline void _write(const char& l, float x, float y, int scale = 1)
{
    if(_font_init == false)
    {
        font = new Texture2D("font.bmp");
        textures.push_back(font);
        texture_aliases["font"] = textures.size() - 1;
        
        _font_init = true;
    }

    blit(font, C(l, scale), x, y);

}
#define cwrite(l, x, y, scale) _write(l, x, y, scale)

inline void _write(const char* text, float x, float y, int scale = 1)
{
    if(!_font_init)
    {
        font = new Texture2D("font.bmp");
        textures.push_back(font);
        texture_aliases["font"] = textures.size() - 1;
        _font_init = true;
    }

    int spacex = 0;

    for (const char* p = text; *p; ++p)
    {
        if (*p != ' ')
        {
            char uc = std::toupper(*p);
            blit(font, C(uc, scale), x + spacex, y);
            spacex += (16 * scale);
        }
        else spacex += 8; 
    }
}

#define swrite(txt, x, y, scale) _write(txt, x, y, scale)
#define write(txt, x, y) _write(txt, x, y)

struct TextBox
{
    float x, y;
    int width, height;
};

inline void _boxwrite(const char* text, const TextBox& box, int scale = 1)
{
    if (!_font_init)
    {
        font = new Texture2D("font.bmp");
        textures.push_back(font);
        texture_aliases["font"] = textures.size() - 1;
        _font_init = true;
    }

    const int char_width = 16 * scale;
    const int char_height = 16 * scale;

    float xpos = 0;
    float ypos = 0;

    for (const char* p = text; *p; ++p)
    {
        if (*p == '\n' || (xpos + char_width > box.width && *p != ' '))
        {
            xpos = 0;
            ypos += char_height;
            if (ypos + char_height > box.height)
                break;

            if (*p == '\n')
                continue;
        }

        if (*p == ' ')
        {
            xpos += 8 * scale;
            continue;
        }

        char uc = std::toupper(*p);
        const auto& glyph = C(uc, scale);
        blit(font, glyph, box.x + xpos, box.y + ypos);
        xpos += char_width;
    }
}

#define boxwrite(txt, x, y, width, height, scale) _boxwrite(txt, {x, y, width, height}, scale)