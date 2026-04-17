#pragma once
#include <string>
#include <vector>
#include <string>
#include <SDL3/SDL.h>
#include <cereal/archives/json.hpp>
#include <box2d/types.h>

// Forward declarations
class Graphics;
class Services;

/**
 * # Vector2
 * A struct which holds an x and y float.
 */
struct Vector2
{
    float x;
    float y;

    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
    explicit Vector2(const b2Vec2& v) {
        x = v.x;
        y = v.y;
    };
    bool operator==(Vector2 o) const { return ( x == o.x && y == o.y ); };
    Vector2 operator*(float s) const { return {x * s, y * s}; }
    Vector2 operator/(float s) const { return {x / s, y / s}; }
    Vector2 operator*(Vector2 s) const { return {x * s.x, y * s.y}; }
    Vector2 operator+(const Vector2 &v) const { return {x + v.x, y + v.y}; }
    Vector2 operator-(const Vector2 &v) const { return {x - v.x, y - v.y}; }
    bool operator!=(Vector2 o) const { return (x != o.x || y != o.y); };
    operator b2Vec2() const { return {x, y}; };

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(x), CEREAL_NVP(y));
    }
};

/**
 * # Texture class
 * Stores an image, its path, its width and its size.
 *
 * Example usage:
 * ```
 * Texture texture = Texture("res://path/to/texture.png");
 * ```
 */
class Texture
{
public:
    Texture() : handle(nullptr), width(0), height(0) {};
    Texture(std::string path);
    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(path));
        this->initialize();
        ar(CEREAL_NVP(width), CEREAL_NVP(height));
    }

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(path), CEREAL_NVP(width), CEREAL_NVP(height));
    }
    void initialize();

    SDL_Texture *handle;
    int width;
    int height;
    std::string path;
};

/**
 * # Matrix2 struct
 * Also known as the bane of my existence :)
 *
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
 *
 * This is the struct that represents the position, rotation and scale of a node (or anything else, for that matter. For example, sprites have source transforms).
 * The position is stored as a Vector2, and the rotation and scale are stored in a Matrix2.
 */
struct Transform
{
    Vector2 position = {0, 0};
    Matrix2 transformation = {{{1, 0}, {0, 1}}};

    void rotate(float angle);
    void scale(Vector2 scale);
    void setAngle(float angle);

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

/**
 * # Polygon
 *
 * A simple polygon class, which basically mirrors the [Box2D polygon class](https://box2d.org/documentation/group__geometry.html#structb2_polygon).
 */
struct Polygon {
    Polygon();
    explicit Polygon(const b2Polygon &polygon);

    Vector2 centroid;
    int count;
    Vector2 normals[B2_MAX_POLYGON_VERTICES];
    float radius;
    Vector2 vertices[B2_MAX_POLYGON_VERTICES];

    operator b2Polygon() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(centroid), CEREAL_NVP(count), CEREAL_NVP(normals), CEREAL_NVP(radius), CEREAL_NVP(vertices));
    }
};