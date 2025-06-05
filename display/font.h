#pragma once

#include <unordered_map>
#include <map>
#include <cctype>
using namespace std;

#include "assets.h"

// STRUCTS
struct TextRegion {
    float x, y;
    int width, height;
};
#define T(x, y, width, height) {x, y, width, height}

// GLOBALS
inline bool font_initialized = false;
inline Texture2D* current_font = nullptr;
inline Texture2D* base_font = nullptr;
inline Texture2D* raw_font = nullptr;
inline std::map<Color, Texture2D*> font_color_cache;
inline Texture2D* active_font_texture = nullptr;

// OPERATORS
inline bool operator<(const Color& lhs, const Color& rhs) {
    if (lhs.r != rhs.r) return lhs.r < rhs.r;
    if (lhs.g != rhs.g) return lhs.g < rhs.g;
    if (lhs.b != rhs.b) return lhs.b < rhs.b;
    return lhs.a < rhs.a;
}

// FUNCTION: Get font surface for a character
inline const Surface& get_font_surface(const char& l, int scale) {
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

    static Surface cached[CHARSET_SIZE][4] = {};
    static bool surface_ready[CHARSET_SIZE][4] = {};

    int s = scale < 1 ? 1 : (scale > 3 ? 3 : scale);

    auto it = char_index.find(l);
    if (it == char_index.end()) {
        static Surface fallback[4] = {
            {{0, 0, LETTER_WIDTH, LETTER_HEIGHT}, {1,1}},
            {{0, 0, LETTER_WIDTH, LETTER_HEIGHT}, {2,2}},
            {{0, 0, LETTER_WIDTH, LETTER_HEIGHT}, {3,3}},
            {{0, 0, LETTER_WIDTH, LETTER_HEIGHT}, {4,4}}
        };
        return fallback[s-1];
    }

    int index = it->second;

    if (!surface_ready[index][s-1]) {
        int col = index % IMAGE_COLS;
        int row = index / IMAGE_COLS;

        cached[index][s-1] = {
            { float(col * LETTER_WIDTH), float(row * LETTER_HEIGHT), LETTER_WIDTH, LETTER_HEIGHT },
            {float(s), float(s)}
        };
        surface_ready[index][s-1] = true;
    }

    return cached[index][s-1];
}

// FUNCTION: Load colorized font texture
inline Texture2D* generate_colored_font(const Color& text_color) {
    if (!base_font) {
        base_font = new Texture2D("font.bmp");
        textures.push_back(base_font);
        texture_aliases["font"] = textures.size() - 1;
    }

    auto it = font_color_cache.find(text_color);
    if (it != font_color_cache.end())
        return it->second;

    auto orig_img = *base_font->image;
    BMP new_img = orig_img;

    int w = new_img.width;
    int h = new_img.height;

    for (int j = 0; j < h; ++j) {
        int rowStart = j * w;
        for (int i = 0; i < w; ++i) {
            int pos = rowStart + i;
            if (new_img.map[pos] != 0) {
                new_img.set(i, j, text_color);
            }
        }
    }

    Texture2D* new_tex = new Texture2D(&new_img);
    textures.push_back(new_tex);
    font_color_cache[text_color] = new_tex;

    return new_tex;
}

// FUNCTION: Font initialization check
inline void ensure_font_loaded() {
    if (!base_font) {
        base_font = new Texture2D("font.bmp");
        textures.push_back(base_font);
        texture_aliases["font"] = textures.size() - 1;
    }
}

// FUNCTION: Raw font init
inline void ensure_raw_font_loaded() {
    if (!raw_font) {
        raw_font = new Texture2D("font_raw.bmp");
        textures.push_back(raw_font);
        texture_aliases["font_raw"] = textures.size() - 1;
    }
}

// FUNCTION: Set font color texture
inline void set_font_color(const Color& color) {
    Texture2D* new_font = generate_colored_font(color);
    if (active_font_texture != new_font) {
        active_font_texture = new_font;
        current_font = new_font;
    }
}

// FUNCTION: Manual spacing adjustment per character
inline std::pair<int,int> compute_char_offset(char c) {
    c = std::toupper(c);
    switch (c) {
        case 'A': case 'B': case 'D': case 'E': case 'F': case 'H':
        case 'K': case 'N': case 'R': case 'T': case 'U': case 'V':
        case 'X': case 'Y': return {0, 2};
        case 'C': case 'G': case 'O': case 'Q': case 'S': case 'Z':
        case '2': case '3': case '5': case '6': case '8': case '9':
        case '?': return {1, 2};
        case 'I': case '1': case '.': case ',': case ':': case '!': return {3, 3};
        case 'J': return {2, 3};
        case 'L': return {2, 2};
        case 'M': case 'W': return {0, 3};
        case '0': case '4': case '7': return {0, 2};
        case ' ': return {0, 8};
        default: return {0, 1};
    }
}

// FUNCTION: Cached spacing
inline std::pair<int,int> get_char_spacing(char c, int scale) {
    static std::unordered_map<char, std::pair<int,int>> spacing_cache;

    char uc = std::toupper(c);
    auto it = spacing_cache.find(uc);
    if (it != spacing_cache.end()) {
        auto [l, r] = it->second;
        return {l * scale, r * scale};
    }

    auto offs = compute_char_offset(uc);
    spacing_cache[uc] = offs;
    return {offs.first * scale, offs.second * scale};
}

// FUNCTION: Print single-line colored text
inline void draw_text(const char* text, float x, float y, const Color& color, int scale) {
    ensure_font_loaded();
    set_font_color(color);

    int xoffset = 0;
    for (const char* p = text; *p; ++p) {
        if (*p != ' ') {
            char uc = std::toupper(*p);
            auto [l_off, r_off] = get_char_spacing(uc, scale);
            blit(current_font, get_font_surface(uc, scale), x + xoffset + l_off, y);
            xoffset += (16 * scale) - r_off;
        } else {
            xoffset += 8 * scale;
        }
    }
}

// FUNCTION: Multiline colored text in box
inline void draw_textbox(const char* text, const TextRegion& region, Color& color, int scale) {
    ensure_font_loaded();
    set_font_color(color);

    const int w = 16 * scale;
    const int h = 16 * scale;

    float xpos = 0, ypos = 0;

    for (const char* p = text; *p; ++p) {
        if (*p == '\n' || (xpos + w > region.width && *p != ' ')) {
            xpos = 0;
            ypos += h;
            if (ypos + h > region.height) break;
            if (*p == '\n') continue;
        }

        if (*p == ' ') {
            xpos += 10 * scale;
            continue;
        }

        char uc = std::toupper(*p);
        blit(current_font, get_font_surface(uc, scale), region.x + xpos, region.y + ypos);
        xpos += w;
    }
}

// FUNCTION: Multiline uncolored font rendering
inline void draw_raw_textbox(const char* text, const TextRegion& region, int scale) {
    ensure_raw_font_loaded();

    const int w = 16 * scale;
    const int h = 16 * scale;

    float xpos = 0, ypos = 0;

    for (const char* p = text; *p; ++p) {
        if (*p == '\n' || (xpos + w > region.width && *p != ' ')) {
            xpos = 0;
            ypos += h;
            if (ypos + h > region.height) break;
            if (*p == '\n') continue;
        }

        if (*p == ' ') {
            xpos += 10 * scale;
            continue;
        }

        char uc = std::toupper(*p);
        blit(raw_font, get_font_surface(uc, scale), region.x + xpos, region.y + ypos);
        xpos += w;
    }
}

// FUNCTION: One-line uncolored font
inline void draw_raw_text(const char* text, float x, float y, int scale) {
    ensure_raw_font_loaded();

    int xoffset = 0;
    for (const char* p = text; *p; ++p) {
        if (*p != ' ') {
            char uc = std::toupper(*p);
            auto [l_off, r_off] = get_char_spacing(uc, scale);
            blit(raw_font, get_font_surface(uc, scale), x + xoffset + l_off, y);
            xoffset += (16 * scale) - r_off;
        } else {
            xoffset += 8 * scale;
        }
    }
}

#define TEXT     draw_text
#define TEXTBOX  draw_textbox
#define RTEXT    draw_raw_text
#define RTBOX    draw_raw_textbox
