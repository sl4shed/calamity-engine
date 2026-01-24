#pragma once
#include <string>
#include <vector>
#include <string>

struct Vector2
{
    float x;
    float y;

    Vector2 operator*(float s) const { return {x * s, y * s}; }
    Vector2 operator+(const Vector2 &v) const { return {x + v.x, y + v.y}; }
    Vector2 operator-(const Vector2 &v) const { return {x - v.x, y - v.y}; }
};

struct Texture
{
    void *handle;
    int width;
    int height;
    std::string path;
};

struct Matrix2
{
    float m[2][2];

    Matrix2 operator*(const Matrix2 &other) const;
    Vector2 operator*(const Vector2 &other) const;
    Matrix2 operator+(const Matrix2 &other) const;
    Matrix2 operator-(const Matrix2 &other) const;
    Matrix2 operator/(float scalar) const;

    static Matrix2 rotation(float angle);
    static Matrix2 scale(Vector2 s);
};

struct Transform
{
    Vector2 position = {0, 0};
    Matrix2 transformation = {{{1, 0}, {0, 1}}};

    void rotate(float angle);
    void scale(Vector2 scale);

    float getAngle();
    float getDegrees();
    Vector2 getScale();
};