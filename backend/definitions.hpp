#pragma once
#include <string>
#include <vector>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cereal/archives/json.hpp>
// Forward declaration
class Graphics;

struct Vector2
{
    float x;
    float y;

    Vector2 operator*(float s) const { return {x * s, y * s}; }
    Vector2 operator+(const Vector2 &v) const { return {x + v.x, y + v.y}; }
    Vector2 operator-(const Vector2 &v) const { return {x - v.x, y - v.y}; }

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(x, y);
    }
};

class Texture
{
public:
    Texture() : handle(nullptr), width(0), height(0) {}
    Texture(Graphics *graphics, std::string path);
    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(path);
    }
    void initialize(Graphics *graphics);

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

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(m);
    }
};

struct Transform
{
    Vector2 position = {0, 0};
    Matrix2 transformation = {{{1, 0}, {0, 1}}};

    void rotate(float angle);
    void scale(Vector2 scale);

    Vector2 applyTo(const Vector2 &point) const;
    Transform applyTo(const Transform &other) const;
    Transform inverse() const;
    float getAngle();
    float getDegrees();
    Vector2 getScale();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(position, transformation);
    }
};