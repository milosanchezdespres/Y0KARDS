#pragma once

#include <unordered_map>
using namespace std;

#include "assets.h"

inline const Surface& letter(const char& l)
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

    static const Surface fontSurface = { {0, 0, 128, 144}, {1.0f, 1.0f} };
    static Surface cache[CHARSET_SIZE] = {};
    static bool initialized[CHARSET_SIZE] = {};

    auto it = char_index.find(l);
    if (it == char_index.end())
    {
        static Surface missing = { {0, 0, LETTER_WIDTH, LETTER_HEIGHT}, {1.0f, 1.0f} };
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

#define C(l) letter(l)

inline bool _font_init = false;
inline Texture2D* font = nullptr;

inline void _write(const char& l, float x, float y)
{
    if(_font_init == false)
    {
        font = new Texture2D("font.bmp");
        textures.push_back(font);
        texture_aliases["font"] = textures.size() - 1;
        
        _font_init = true;
    }

    blit(font, C(l), x, y);

}
#define write(l, x, y) _write(l, x, y)