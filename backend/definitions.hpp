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

struct Transform
{
    Vector2 position;
    Vector2 scale;
    Vector2 origin;
    float angle;

    Matrix3 transformation;
};

struct Matrix3
{
    float m[3][3];

    Matrix3 operator*(const Matrix3 &other) const;
    Matrix3 operator+(const Matrix3 &other) const;
    Matrix3 operator-(const Matrix3 &other) const;
    Matrix3 operator/(float scalar) const;

    static Matrix3 translation(Vector2 t);
    static Matrix3 rotation(float angle);
    static Matrix3 scale(Vector2 s);
}