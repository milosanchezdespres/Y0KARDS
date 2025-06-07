#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
using namespace std;

struct File
{
    string path;
    vector<uint8_t> bytes;
};

struct _pixel { uint8_t r, g, b; };

struct BMP : File
{
    vector<int> pixels;
    vector<uint32_t> palette;

    uint32_t begin;

    uint32_t width, height;
    int row_byte_size;

    uint16_t bpp;

    uint32_t pixel(int i, int j)
    {
        int index = i * width + j;

        uint32_t color = palette[pixels[index]];

        return color;
    }

    void edit(int i, int j, _pixel _color)
    {
        int index = i * width + j;

        uint32_t color = (_color.r << 16) | (_color.g << 8) | _color.b;

        auto it = find(palette.begin(), palette.end(), color);

        if (it == palette.end())
        {
            palette.push_back(color);
            it = palette.end() - 1;
        }

        int color_index = distance(palette.begin(), it);

        pixels[index] = color_index;
    }
};

inline uint32_t  byte_to_uint32_t(const vector<uint8_t>& bytes, const array<int, 4>& byte_indices, const array<int, 4>& offsets)
{
    return
        ((uint32_t)bytes[byte_indices[0]] << offsets[0]) |
        ((uint32_t)bytes[byte_indices[1]] << offsets[1]) |
        ((uint32_t)bytes[byte_indices[2]] << offsets[2]) |
        ((uint32_t)bytes[byte_indices[3]] << offsets[3]);
}

inline File load_bytes(const char* path)
{
    File file;
    file.path = path;

    ifstream file_stream(path, ios::binary);

    file_stream.seekg(0, std::ios::end);
    streamsize size = file_stream.tellg();
    file_stream.seekg(0, ios::beg);

    file.bytes.resize(size);

    if (!file_stream.read((char*) file.bytes.data(), size)) { cerr << "Failed to read file\n"; }

    return file;
}

inline BMP load_bmp(string path)
{
    string full_path = path + ".bmp";

    File _file = load_bytes(full_path.c_str());
    BMP file;

    file.path = _file.path;
    file.bytes = _file.bytes;

    file.begin = byte_to_uint32_t(file.bytes, {10, 11, 12, 13}, {0, 8, 16, 24});
    file.width = byte_to_uint32_t(file.bytes, {18, 19, 20, 21}, {0, 8, 16, 24});
    file.height = byte_to_uint32_t(file.bytes, {22, 23, 24, 25}, {0, 8, 16, 24});

    file.bpp = (uint16_t) file.bytes[28] | ((uint16_t) file.bytes[29] << 8);
    file.row_byte_size =  ((file.bpp * file.width + 31) / 32) * 4;

    for (int row = file.height - 1; row >= 0; --row)
    {
        int row_start = file.begin + row * file.row_byte_size;

        for (int col = 0; col < file.width; ++col)
        {
            int pixel_start = row_start + col * 3;

            uint8_t b  = file.bytes[pixel_start];
            uint8_t g = file.bytes[pixel_start + 1];
            uint8_t r   = file.bytes[pixel_start + 2];
            uint8_t a = 255;

            uint32_t color = (r << 16) | (g << 8) | b;

            auto it = find(file.palette.begin(), file.palette.end(), color);
            if (it == file.palette.end()) file.palette.push_back(color);

            int color_index = distance(file.palette.begin(), it);
            file.pixels.push_back(color_index);
        }
    }

    return file;
}