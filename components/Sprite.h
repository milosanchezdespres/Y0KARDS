#pragma once

#include "../ecs.h"
#include "../display/display.h"

struct Sprite : public Component
{
    Surface surface;
    Texture2D* texture = nullptr;

    int tile_width, tile_height;
    int _local_scale;
    
    Sprite() : Component() {}

    void set(int frame_index, int _tile_width, int _tile_height, int scale = 1)
    {
        _local_scale = scale;

        tile_width = _tile_width;
        tile_height = _tile_height;

        int texWidth = texture->width;
        int texHeight = texture->height;

        int cols = texWidth / tile_width;
        int rows = texHeight / tile_height;

        int totalFrames = cols * rows;

        if (frame_index < 0) frame_index = 0;
        if (frame_index >= totalFrames) frame_index = totalFrames - 1;

        int col = frame_index % cols;
        int row = frame_index / cols;

        surface.data.x = float(col * tile_width);
        surface.data.y = float(row * tile_height);
        surface.data.width = tile_width;
        surface.data.height = tile_height;

        surface.scale.x = _local_scale;
        surface.scale.y = _local_scale;
    }

    void set(int frame_index)
    {
        tile_width = tile_width;
        tile_height = tile_height;

        int texWidth = texture->width;
        int texHeight = texture->height;

        int cols = texWidth / tile_width;
        int rows = texHeight / tile_height;

        int totalFrames = cols * rows;

        if (frame_index < 0) frame_index = 0;
        if (frame_index >= totalFrames) frame_index = totalFrames - 1;

        int col = frame_index % cols;
        int row = frame_index / cols;

        surface.data.x = float(col * tile_width);
        surface.data.y = float(row * tile_height);
        surface.data.width = tile_width;
        surface.data.height = tile_height;

        surface.scale.x = _local_scale;
        surface.scale.y = _local_scale;
    }

    const char* default_name() override{ return "sprite"; }
};