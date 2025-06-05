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
inline Surface get_font_surface(const char& l, float scale) {
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

    auto it = char_index.find(l);
    if (it == char_index.end()) {
        return { {0, 0, LETTER_WIDTH, LETTER_HEIGHT}, {scale, scale} };
    }

    int index = it->second;
    int col = index % IMAGE_COLS;
    int row = index / IMAGE_COLS;

    return {
        { float(col * LETTER_WIDTH), float(row * LETTER_HEIGHT), LETTER_WIDTH, LETTER_HEIGHT },
        {scale, scale}
    };
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
inline std::pair<float, float> compute_char_offset(char c) {
    c = std::toupper(c);
    switch (c) {
        case '.': return {1, 2};
        case 'I': case '1': case ',': case ':': case '!': return {-3, 7};
        case 'J': return {2.0f, 1.2f};
        case 'M': case 'W': return {0.2f, 4.0f};
        case ' ': return {0.0f, 8.0f};
        case 'A': case 'V': return {0.3f, 0.8f};
        case 'T': return {0.8f, 1.0f};
        default: return {1.0f, 1.0f};
    }
}

// FUNCTION: Cached spacing
inline std::pair<float, float> get_char_spacing(char c, float scale) {
    static std::unordered_map<char, std::pair<float, float>> spacing_cache;

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

// FUNCTION: Print single-line colored text with >> as newline
inline void draw_text(const char* text, float x, float y, const Color& color, float scale) {
    ensure_font_loaded();
    set_font_color(color);

    float xoffset = 0;
    float yoffset = 0;
    for (const char* p = text; *p; ) {
        if (p[0] == '>' && p[1] == '>') {
            xoffset = 0;
            yoffset += 16.0f * scale;
            p += 2;
            continue;
        }

        if (*p != ' ') {
            char uc = std::toupper(*p);
            auto [l_off, r_off] = get_char_spacing(uc, scale);
            blit(current_font, get_font_surface(uc, scale), x + xoffset + l_off, y + yoffset);
            xoffset += (16.0f * scale) - r_off;
        } else {
            xoffset += 8.0f * scale;
        }
        ++p;
    }
}

// FUNCTION: Multiline colored text in box with >> as newline and spacing offsets
inline void draw_textbox(const char* text, const TextRegion& region, Color& color, float scale) {
    ensure_font_loaded();
    set_font_color(color);

    const float h = 16.0f * scale;

    float xpos = 0, ypos = 0;

    for (const char* p = text; *p; ) {
        if (p[0] == '>' && p[1] == '>') {
            xpos = 0;
            ypos += h;
            if (ypos + h > region.height) break;
            p += 2;
            continue;
        }

        if (*p == '\n' || (xpos + 16.0f * scale > region.width && *p != ' ')) {
            xpos = 0;
            ypos += h;
            if (ypos + h > region.height) break;
            if (*p == '\n') {
                ++p;
                continue;
            }
        }

        if (*p == ' ') {
            xpos += 10.0f * scale;
            ++p;
            continue;
        }

        char uc = std::toupper(*p);
        auto [l_off, r_off] = get_char_spacing(uc, scale);
        blit(current_font, get_font_surface(uc, scale), region.x + xpos + l_off, region.y + ypos);
        xpos += (16.0f * scale) - r_off;
        ++p;
    }
}

// FUNCTION: Multiline uncolored font rendering with >> as newline and spacing offsets
inline void draw_raw_textbox(const char* text, const TextRegion& region, float scale) {
    ensure_raw_font_loaded();

    const float h = 16.0f * scale;

    float xpos = 0, ypos = 0;

    for (const char* p = text; *p; ) {
        if (p[0] == '>' && p[1] == '>') {
            xpos = 0;
            ypos += h;
            if (ypos + h > region.height) break;
            p += 2;
            continue;
        }

        if (*p == '\n' || (xpos + 16.0f * scale > region.width && *p != ' ')) {
            xpos = 0;
            ypos += h;
            if (ypos + h > region.height) break;
            if (*p == '\n') {
                ++p;
                continue;
            }
        }

        if (*p == ' ') {
            xpos += 10.0f * scale;
            ++p;
            continue;
        }

        char uc = std::toupper(*p);
        auto [l_off, r_off] = get_char_spacing(uc, scale);
        blit(raw_font, get_font_surface(uc, scale), region.x + xpos + l_off, region.y + ypos);
        xpos += (16.0f * scale) - r_off;
        ++p;
    }
}

// FUNCTION: One-line uncolored font with >> as newline (rare usage, splits line)
inline void draw_raw_text(const char* text, float x, float y, float scale) {
    ensure_raw_font_loaded();

    float xoffset = 0;
    float yoffset = 0;
    for (const char* p = text; *p; ) {
        if (p[0] == '>' && p[1] == '>') {
            xoffset = 0;
            yoffset += 16.0f * scale;
            p += 2;
            continue;
        }

        if (*p != ' ') {
            char uc = std::toupper(*p);
            auto [l_off, r_off] = get_char_spacing(uc, scale);
            blit(raw_font, get_font_surface(uc, scale), x + xoffset + l_off, y + yoffset);
            xoffset += (16.0f * scale) - r_off;
        } else {
            xoffset += 8.0f * scale;
        }
        ++p;
    }
}

#define TEXT     draw_text
#define TEXTBOX  draw_textbox
#define RTEXT    draw_raw_text
#define RTBOX    draw_raw_textbox
