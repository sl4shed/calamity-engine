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
 * this is needed for physics scale and to avoid a stupid circular import
 */
struct PhysicsConstants
{
    static constexpr float scale = 0.01f; // 100 pixels = 1 meter
};

/**
 * # Color
 * A simple class that defines a color using red, green, blue and alpha channels.
 *
 * You can construct it with a string hex code, an actual hex code, or provide individual channel values up to 255.
 */
class Color {
public:
    Uint8 r; // Red value. Goes up to 255.
    Uint8 g; // Green value. Goes up to 255.
    Uint8 b; // Blue value. Goes up to 255.
    Uint8 a; // Alpha value. Goes up to 255.

    Color(int r, int g, int b);
    Color(int r, int g, int b, int a);
    Color(int hexCode);
    Color(int hexCode, int a);
    Color(std::string hexCode);
    Color(const std::string& hexCode, int a);

    operator SDL_Color() const { return {r, g, b, a}; };
    operator SDL_FColor() const { return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f}; };

    static const Color WHITE;
    static const Color BLACK;
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color TRANSPARENT;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(r, g, b, a);
    }
};

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
    Vector2 operator/(Vector2 s) const { return {x / s.x, y / s.y}; };
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

enum class TextureScaling
{
    INVALID = -1,
    NEAREST,
    LINEAR,
    PIXELART
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
    Texture() : handle(nullptr), width(0), height(0), textureWidth(0), textureHeight(0) {};
    Texture(const std::string& path, TextureScaling scaling = TextureScaling::NEAREST);
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept : handle(other.handle), width(other.width), height(other.height), textureWidth(other.textureWidth), textureHeight(other.textureHeight), path(std::move(other.path))
    {
        other.handle = nullptr;
    }

    Texture& operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            SDL_DestroyTexture(handle);
            handle = other.handle;
            width = other.width;
            height = other.height;
            textureWidth = other.textureWidth;
            textureHeight = other.textureHeight;
            path = std::move(other.path);
            other.handle = nullptr;
        }
        return *this;
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(path), CEREAL_NVP(scaling));
        this->initialize();
        ar(CEREAL_NVP(width), CEREAL_NVP(height));
    }

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(path), CEREAL_NVP(scaling), CEREAL_NVP(width), CEREAL_NVP(height));
    }
    void initialize();

    SDL_Texture *handle;
    int width;
    int height;
    int textureWidth;
    int textureHeight;
    std::string path;
private:
    TextureScaling scaling;
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
    void rotateRadians(float radians);
    void setScale(Vector2 scale);
    void scale(Vector2 scale);
    void setAngle(float angle);
    void setAngleRadians(float radians);

    Vector2 applyTo(const Vector2 &point) const;
    Transform applyTo(const Transform &other) const;
    Transform inverse() const;
    float getAngle() const;
    float getAngleRadians() const;
    float getDegrees() const;
    Vector2 getScale() const;

    static float degToRad(float degrees);
    static float radToDeg(float radians);

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(position), CEREAL_NVP(transformation));
    }
};

/**
 * # Rect
 * A simple rect class. It detrermines two Vector2's: position and size.
 */
struct Rect
{
    Rect() {};
    Rect(const Vector2 _position, const Vector2 _size) : position(_position), size(_size) {};
    Vector2 position = {0, 0};
    Vector2 size = {0, 0};

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(position), CEREAL_NVP(size));
    }

    operator SDL_Rect() const
    {
        return SDL_Rect{static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(size.x), static_cast<int>(size.y)};
    };
};

/**
 * # Frame
 * An animation frame. It determines the source rectangle of the sprite, the origin of the rendered sprite and the modulate color of the rendered sprite.
 */
struct Frame
{
    Frame() = default;
    Frame(const Rect _rect, const Vector2 _origin = {0.5, 0.5f}, const Color _modulate = Color::WHITE) : rect(_rect), origin(_origin), modulate(_modulate) {};

    Rect rect;
    Vector2 origin = {0.5f, 0.5f};
    Color modulate = Color::WHITE;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(rect), CEREAL_NVP(origin), CEREAL_NVP(modulate));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(rect), CEREAL_NVP(origin), CEREAL_NVP(modulate));
    }
};

/**
 * # Animation
 * An animation that is used to AnimatedSprite.
 */
class Animation
{
public:
    Animation(std::string _name = "Animation", int _fps = 30, Vector2 _size = {0, 0}, bool _loop = true, bool _autoplay = false) : name(_name), fps(_fps), size(_size), loop(_loop), autoplay(_autoplay) {};

    std::string name;
    int fps = 15;
    std::vector<Frame> frames;

    std::string texturePath;
    TextureScaling textureScaling;

    Vector2 size;
    bool loop = true;
    bool autoplay = false;

    template <typename... Args>
    void addFrames(Args... frames)
    {
        (this->frames.push_back(frames), ...);
    }

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(fps), CEREAL_NVP(frames), CEREAL_NVP(texturePath), CEREAL_NVP(loop), CEREAL_NVP(autoplay));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(fps), CEREAL_NVP(frames), CEREAL_NVP(texturePath), CEREAL_NVP(loop), CEREAL_NVP(autoplay));
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

/**
 * # Circle
 *
 * A simple circle class, which basically mirrors the Box2D circle class.
 */
struct Circle
{
    Circle();
    explicit Circle(const b2Circle &circle);

    Vector2 center = {0.5f, 0.5f};
    float radius;

    operator b2Circle() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(center), CEREAL_NVP(radius));
    }
};

struct Capsule
{
    Capsule();
    explicit Capsule(const b2Capsule &capsule);

    Vector2 center1;
    Vector2 center2;
    float radius;

    operator b2Capsule() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(center1), CEREAL_NVP(center2), CEREAL_NVP(radius));
    }
};

struct Tile
{
    Tile(Vector2 gridPosition, Rect sourceRect = {{0, 0}, {64, 64}}, Color modulate = Color::WHITE) : gridPosition(gridPosition), sourceRect(sourceRect), modulate(modulate) {};

    Vector2 gridPosition;
    Rect sourceRect;
    Color modulate;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(gridPosition, sourceRect, modulate);
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(gridPosition, sourceRect, modulate);
    }
};