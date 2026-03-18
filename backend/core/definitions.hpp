#pragma once
#include <string>
#include <vector>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cereal/archives/json.hpp>
// Forward declarations
class Graphics;
class Services;

/**
 * # Vector2 Struct
 * Literally just like any other Vector2 struct you have ever seen in your lifetime.
 * You can multiply, add and subtract them for now.
 */
struct Vector2
{
    float x;
    float y;

    Vector2 operator*(float s) const { return {x * s, y * s}; }
    Vector2 operator*(Vector2 s) const { return {x * s.x, y * s.y}; }
    Vector2 operator+(const Vector2 &v) const { return {x + v.x, y + v.y}; }
    Vector2 operator-(const Vector2 &v) const { return {x - v.x, y - v.y}; }

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(x), CEREAL_NVP(y));
    }
};

/**
 * # Texture class
 * This is a simple wrapper around an SDL_Texture pointer, which also stores the width and height of the texture.
 * Keep in mind when serializing this it does not serialize the SDL_Texture pointer but only the path of the texture to be efficient.
 * (also because SDL_Texture pointers can't be serialized, but you get the point)
 *
 * The texture is initialized automatically after loading, so don't worry about calling ``initialize()``.
 */
class Texture
{
public:
    Texture() : handle(nullptr), width(0), height(0) {}
    Texture(std::string path);
    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(path));
        this->initialize();
    }

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(path));
    }
    void initialize();

    void *handle;
    int width;
    int height;
    std::string path;
};

/**
 * # Matrix2 struct
 * also known as the bane of my existence. nothing interesting here either.
 * You can multiply them together, multiply with a Vector2, subtract, divide and add them.
 * You can also create rotation and scale matrices with the static functions.
 */
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
        ar(cereal::make_nvp("matrix", m));
    }
};

/**
 * # Transform struct
 * This is the struct that represents the position, rotation and scale of a node.
 * The position is stored as a Vector2, and the rotation and scale are stored in a Matrix2.
 *
 * For simplicity's sake, you can kindly ignore the ``applyTo()`` function
 * (it just applies the transformation matrix to another transform or Vector2) because it's for internal use :)
 */
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
        ar(CEREAL_NVP(position), CEREAL_NVP(transformation));
    }
};