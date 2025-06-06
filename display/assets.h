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

//use this when you wanna use the alias in an entire file, not just scope wise.
//but you must have a pointer to it somewhere !
#define gload(alias, path) \
    do { \
        if (alias) delete alias; \
        alias = new Texture2D(path); \
        textures.push_back(alias); \
        texture_aliases[#alias] = textures.size() - 1; \
    } while(0)

inline void free_all_textures()
{
    for (Texture2D* tex : textures)
        delete tex;
    textures.clear();
    texture_aliases.clear();
}
#define FREE_ASSETS free_all_textures()