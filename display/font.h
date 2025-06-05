#pragma once

#include <unordered_map>
#include <cctype>
using namespace std;

#include "assets.h"

inline const Surface& letter(const char& l, int scale)
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

    static Surface cache[CHARSET_SIZE][4] = {};
    static bool initialized[CHARSET_SIZE][4] = {};

    int s = scale < 1 ? 1 : (scale > 3 ? 3 : scale);

    auto it = char_index.find(l);
    if (it == char_index.end())
    {
        static Surface missing[4] = {
            {{0, 0, LETTER_WIDTH, LETTER_HEIGHT}, {1,1}},
            {{0, 0, LETTER_WIDTH, LETTER_HEIGHT}, {2,2}},
            {{0, 0, LETTER_WIDTH, LETTER_HEIGHT}, {3,3}},
            {{0, 0, LETTER_WIDTH, LETTER_HEIGHT}, {4,4}}
        };
        return missing[s-1];
    }

    int index = it->second;

    if (!initialized[index][s-1])
    {
        int col = index % IMAGE_COLS;
        int row = index / IMAGE_COLS;

        cache[index][s-1] = {
            { float(col * LETTER_WIDTH), float(row * LETTER_HEIGHT), LETTER_WIDTH, LETTER_HEIGHT },
            {float(s), float(s)}
        };
        initialized[index][s-1] = true;
    }

    return cache[index][s-1];
}

#define C(l, scale) letter(l, scale)

inline bool _font_init = false;
inline Texture2D* font = nullptr;

inline void _font_init_check()
{
    if (!_font_init)
    {
        font = new Texture2D("font.bmp");
        textures.push_back(font);
        texture_aliases["font"] = textures.size() - 1;
        _font_init = true;
    }
}

inline void _set_font_color_if_needed(const Color& text_color)
{
    if (text_color != __fontcolor)
    {
        int w = font->image->width;
        int h = font->image->height;
        auto& img = *font->image;

        for (int j = 0; j < h; ++j)
        {
            int rowStart = j * w;
            for (int i = 0; i < w; ++i)
            {
                int pos = rowStart + i;
                if (img.map[pos] != 0)
                    img.set(i, j, text_color);
            }
        }
        font->refresh();

        __fontcolor = text_color;
    }
}

inline std::pair<int,int> compute_offset(char c)
{
    c = std::toupper(c);
    switch (c)
    {
        case 'A': return {0, 2};
        case 'B': return {0, 2};
        case 'C': return {1, 2};
        case 'D': return {0, 2};
        case 'E': return {0, 2};
        case 'F': return {0, 2};
        case 'G': return {1, 2};
        case 'H': return {0, 2};
        case 'I': return {4, 4};
        case 'J': return {2, 3};
        case 'K': return {0, 2};
        case 'L': return {2, 2};
        case 'M': return {0, 3};
        case 'N': return {0, 2};
        case 'O': return {1, 2};
        case 'P': return {0, 2};
        case 'Q': return {1, 2};
        case 'R': return {0, 2};
        case 'S': return {1, 2};
        case 'T': return {1, 4};
        case 'U': return {0, 2};
        case 'V': return {0, 2};
        case 'W': return {0, 3};
        case 'X': return {0, 2};
        case 'Y': return {0, 2};
        case 'Z': return {1, 2};
        case '0': return {1, 2};
        case '1': return {3, 3};
        case '2': return {1, 2};
        case '3': return {1, 2};
        case '4': return {0, 2};
        case '5': return {1, 2};
        case '6': return {1, 2};
        case '7': return {0, 2};
        case '8': return {1, 2};
        case '9': return {1, 2};
        case '.': return {3, 3};
        case ',': return {3, 3};
        case ':': return {3, 3};
        case '!': return {3, 3};
        case '?': return {1, 3};
        case ' ': return {0, 8};
        default: return {0, 1};
    }
}

inline std::pair<int,int> get_letter_offsets(char c, int scale)
{
    static std::unordered_map<char, std::pair<int,int>> cache;

    char uc = std::toupper(c);

    auto it = cache.find(uc);
    if(it != cache.end())
    {
        auto [l, r] = it->second;
        return {l * scale, r * scale};
    }

    auto offs = compute_offset(uc);
    cache[uc] = offs;

    return {offs.first * scale, offs.second * scale};
}

inline void _write(const char* text, float x, float y, const Color& text_color, int scale)
{
    _font_init_check();
    _set_font_color_if_needed(text_color);

    int spacex = 0;
    for (const char* p = text; *p; ++p)
    {
        if (*p != ' ')
        {
            char uc = std::toupper(*p);
            auto [leftOffset, rightOffset] = get_letter_offsets(uc, scale);
            blit(font, C(uc, scale), x + spacex + leftOffset, y);
            spacex += (16 * scale) - rightOffset;
        }
        else
        {
            spacex += 8 * scale;
        }
    }
}

#define print(txt, x, y, text_color, scale) _write(txt, x, y, text_color, scale)

struct TextBox
{
    float x, y;
    int width, height;
};

inline void _boxwrite(const char* text, const TextBox& box, Color& text_color, int scale)
{
    _font_init_check();
    _set_font_color_if_needed(text_color);

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
            xpos += 10 * scale;
            continue;
        }

        char uc = std::toupper(*p);
        blit(font, C(uc, scale), box.x + xpos, box.y + ypos);
        xpos += char_width;
    }
}

#define printb(txt, x, y, width, height, text_color, scale) _boxwrite(txt, {x, y, width, height}, text_color, scale)
