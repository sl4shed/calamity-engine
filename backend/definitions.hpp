#pragma once
#include <string>
#include <vector>
#include <string>

struct Vector2D
{
    float x;
    float y;

    Vector2D operator*(float s) const { return {x * s, y * s}; }
    Vector2D operator+(const Vector2D &v) const { return {x + v.x, y + v.y}; }
    Vector2D operator-(const Vector2D &v) const { return {x - v.x, y - v.y}; }
};

struct Texture
{
    void *handle;
    int width;
    int height;
    std::string path;
};

struct Transform
{
    Vector2D position;
    Vector2D scale;
    Vector2D origin;
    float angle;
};