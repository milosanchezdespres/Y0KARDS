#pragma once

#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

#include "../display/texture2d.h"

inline vector<Texture2D*> textures;
inline unordered_map<const char* , int> texture_aliases;

#define load(alias, path) \
    Texture2D* alias = new Texture2D(path); \
    textures.push_back(alias); \
    texture_aliases[#alias] = textures.size() - 1;

#define S(tilex, tiley, tilew, tileh, scalex, scaley) {{tilex, tiley, tilew, tileh}, {scalex, scaley}}