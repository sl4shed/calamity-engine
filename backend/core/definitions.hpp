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
 * # this is needed for physics scale and to avoid a stupid circular import
 * # meant for internal use
 */
struct PhysicsConstants
{
    static constexpr float scale = 0.01f; // 100 pixels = 1 meter
};

/**
 * # Color
 * A simple class that defines a color using red, green, blue and alpha channels.
 *
 * Example usage:
 * ```cpp
 * Color rgbColor = Color(255, 0, 0);
 * Color rgbaColor = Color(255, 255, 0, 100);
 * Color hexColor = Color(0xFFFFFF);
 * Color ahexColor = Color(0xFFFFFF, 100); // second argument is the alpha value
 * Color shexColor = Color("#FFFFFF");
 * Color sahexColor = Color("#FFFFFF", 100); // second argument is alpha value
 * ```
 * 
 * It also defines several pre-made static colors, like `Color::WHITE`, `Color::BLACK`, `Color::TRANSPARENT`, etc...
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
 * A struct which holds an x and y float. Used for a lot of things across Calamity Engine.
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

/**
 * # Texture Scaling
 * This enum defines how the texture is rendered and scaled.
 * 
 * Linear scaling will smooth the values between pixels, while nearest neighbour scaling will use an average. Pixel art scaling exists due to SDL3, and I recommend you use it for pixel art textures instead of nearest neighbour.
 */
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
 * 
 * ## Properties and usage
 * 
 * You can set the \ref TextureScaling ["TextureScaling"] of the texture:
 * ```cpp
 * Texture texture = Texture("res://path/to/texture.png", TextureScaling::PIXELART);
 * ```
 * 
 * You can also define the size of the rendered texture on screen!
 * ```cpp
 * texture.width = 64;
 * texture.height = 64;
 * 
 * // Now the texture will be 64x64 pixels when rendered, even if the texture is bigger or smaller.
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
 * This is the struct that represents the position, rotation and scale of a node among other things.
 * The position is stored as a Vector2, and the rotation and scale are stored in a transformation matrix.
 * 
 * There are a lot of operations you can do to a Transform:
 * ```cpp
 * Transform myTransform;
 * myTransform.position = Vector2{0.0f, 0.0f};
 * myTransform.setScale(Vector2{3.0f, 3.0f}); // make it 3 times as big!
 * 
 * Logger::log("{}", myTransform.getAngleRadians()); // print out the angle of the Transform in radians!
 * myTransform.setAngleRadians(20.0f * (3.141592653f / 180.0f)) // set the angle to 20 degrees in radians
 * 
 * myTransform.rotate(-20.0f); // rotate it back 20 degrees, and now the rotation will be 0 degrees.
 * ```
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
 * A simple rect class. It detrermines two Vector2's: position and size. Its used mainly to define sourceRects of \ref Sprite ["Sprites"].
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
 * 
 * Example usage:
 * ```cpp
 * // This will create a frame a sourceRect which has the position of 0, 0 in the texture atlas and the size of 16x16 pixels.
 * // It has an origin of {0.5f, 0.5f} and a modulate color of blue.
 * Frame(Rect{Vector2{0.0f, 0.0f}, Vector2{16.0f, 16.0f}}, {0.5f, 0.5f}, Color::BLUE);
 * ```
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
 * The Animation component represents a series of \ref Frame ["Frames"]. It is used with \ref AnimatedSprite ["AnimatedSprites"].
 * 
 * Example usage:
 * ```
 * // Create an animation which has the name "myAnimationName", runs at 15 FPS, has the size of 20x20 pixels, does not loop and does not autoplay.
 * Animation anim = Animation("myAnimationName", 15, Vector2{20.0f, 20.0f}, false, false);
 * ```
 * 
 * ## Properties and usages
 * 
 * For an animation to work on an AnimatedSprite, you must set it's texturePath:
 * ```cpp
 * anim.texturePath = "res://path/to/texture.png";
 * ```
 * Instead of directly storing a texture, Animations only store the path and scaling of the texture.
 * The texture is eventually loaded when playing the animation to save on memory.
 * 
 * You can also define the TextureScaling type:
 * ```cpp
 * anim.textureScaling = TextureScaling::PIXELART;
 * ```
 * 
 * Make sure to also check out the [animated sprite example](https://calamity.sl4shed.xyz/example-animated-sprite)!
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
 * A simple polygon class, which basically mirrors the [Box2D polygon class](https://box2d.org/documentation/group__geometry.html#structb2_polygon).
 *
 * By the way, you should never manually fill these out. Instead, when using \ref PolygonShape ["PolygonShapes"], use the PolygonShapes constructor.
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
 * A simple circle class.
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

/**
 * # Capsule
 * The capsule class defines the two ends of the capsule (in \ref Vector2 ["Vector2s"]), and the radius of the end circles.
 */
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

/**
 * # Tile
 * 
 * The tile class defines the position of the tile on the Tilemap grid, the sourceRect of the texture atlas and the modulate Color of the Tile.
 * 
 * Example usage:
 * ```
 * // Create a tile at the gridPosition of `{-1, 0}`.
 * Tile myTile = Tile(Vector2{-1.0f, 0.0f}, Rect{Vector2{0.0f, 0.0f}, Vector2{64.0f, 64.0f}}, Color::RED);
 * ```
 */
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