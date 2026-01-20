#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <string>
#include <vector>
#include <string>

struct Vector2D
{
    float x;
    float y;
};

struct Texture
{
    void *handle;
    int width;
    int height;
    std::string path;
};

#endif
