#pragma once
#include <cereal/types/polymorphic.hpp>

#include "../../core/definitions.hpp"
#include "../../core/node/components.hpp"

/**
 * # Material
 * The Material class defines Physics properties for any Shape.
 * To apply a material onto a Shape, use the `applyMaterial()` function of the Shape.
 */
class Material
{
public:
    float density = 1.0f;
    float friction = 0.3f;
    float restitution = 0.0f;
    float rollingResistance = 0.0f;
    float tangentSpeed = 0.0f;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(density), CEREAL_NVP(friction), CEREAL_NVP(restitution), CEREAL_NVP(rollingResistance), CEREAL_NVP(tangentSpeed));
    }
};

/**
 * # Shape
 * The base Shape class defines properties which are inherited by all Shapes, such as BoxShape, CircleShape, CapsuleShape and PolygonShape.
 * 
 * This class has two notable properties:
 * ```cpp
 * shape->origin = {0.5f, 0.5f}; // The origin of the shape. This is disabled for PolygonShape.
 * shape->applyMaterial(myMaterial); // This is the main method of applying materials to shapes.
 * ```
 */
class Shape
{
public:
    virtual ~Shape() = default;
    b2ShapeDef shapeDef;
    Vector2 origin;

    void applyMaterial(const Material& material);

    Shape *setSensor(bool sensor)
    {
        shapeDef.isSensor = sensor;
        if (sensor)
        {
            shapeDef.enableSensorEvents = true;
        }
        return this;
    }

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(material), CEREAL_NVP(origin));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(material), CEREAL_NVP(origin));
    }

private:
    Material material;
};

class BoxShape : public Shape
{
public:
    BoxShape() = default;
    BoxShape(Vector2 size, Vector2 origin = {0.5f, 0.5f});
    Vector2 size;

    Polygon scaledPolygon;
    Polygon polygon;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::base_class<Shape>(this), CEREAL_NVP(size), CEREAL_NVP(scaledPolygon), CEREAL_NVP(polygon));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(cereal::base_class<Shape>(this), CEREAL_NVP(size), CEREAL_NVP(scaledPolygon), CEREAL_NVP(polygon));

        // just reconstruct everything from size and center like the constructor does
        Vector2 calculatedCenter = origin * size;
        b2Rot rotation = {cos(0.0f), sin(0.0f)};

        b2Polygon poly = b2MakeOffsetBox(size.x / 2 * PhysicsConstants::scale, size.y / 2 * PhysicsConstants::scale, (calculatedCenter * PhysicsConstants::scale), rotation);
        b2Polygon polyUnscaled = b2MakeOffsetBox(size.x / 2, size.y / 2, calculatedCenter, rotation);

        this->polygon = static_cast<Polygon>(polyUnscaled);
        this->scaledPolygon = static_cast<Polygon>(poly);
        this->shapeDef = b2DefaultShapeDef();
    }
};

class CircleShape : public Shape
{
public:
    CircleShape() = default;
    CircleShape(float radius, Vector2 center = {0, 0}, Vector2 origin = {0.5f, 0.5f});
    float radius{};

    Circle circle;
    Circle scaledCircle;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::base_class<Shape>(this), CEREAL_NVP(radius), CEREAL_NVP(circle));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(cereal::base_class<Shape>(this), CEREAL_NVP(radius), CEREAL_NVP(circle));
    }
};

class CapsuleShape : public Shape
{
public:
    CapsuleShape() = default;
    CapsuleShape(Vector2 center1, Vector2 center2, float radius, Vector2 origin);

    Capsule capsule;
    Capsule scaledCapsule;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::base_class<Shape>(this), CEREAL_NVP(capsule), CEREAL_NVP(scaledCapsule));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(cereal::base_class<Shape>(this), CEREAL_NVP(capsule), CEREAL_NVP(scaledCapsule));
    }
};

class PolygonShape : public Shape
{
public:
    PolygonShape() = default;
    PolygonShape(std::vector<Vector2> points);

    Polygon polygon;
    Polygon scaledPolygon;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::base_class<Shape>(this), CEREAL_NVP(scaledPolygon), CEREAL_NVP(polygon));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(cereal::base_class<Shape>(this), CEREAL_NVP(scaledPolygon), CEREAL_NVP(polygon));
        this->shapeDef = b2DefaultShapeDef();
    }
};


/**
 * # ShapeSprite
 * A dynamic sprite (mainly used for debugging) which can draw solid color filled shapes.
 *
 * Example usage:
 * ```cpp
 * std::shared_ptr<Node> spriteNode = std::make_shared<Node>();
 * auto boxShape = std::make_shared<BoxShape>(Vector2{0.5f, 0.5f});
 * std::shared_ptr<ShapeSprite> sprite = std::make_shared<ShapeSprite>(shape);
 * spriteNode->addComponent(sprite);
 * ```
 * 
 * ## Properties and usages
 * 
 * You can make ShapeSprites render using screen space positioning. This can be used for UI elements and things like that:
 * ```cpp
 * sprite->screenSpace = true; // Now, the position of the sprite will directly translate to screen coordinates!
 * ```
 * 
 * Also, you can modify the modulate variable of the sprite to change its color:
 * ```cpp
 * sprite->color = Color::RED;
 * ```
 */
class ShapeSprite : public Component
{
public:
    ShapeSprite();
    explicit ShapeSprite(const std::shared_ptr<Shape>& shape);

    Vector2 origin = {0.5f, 0.5f};
    std::shared_ptr<Shape> shape;
    bool visible = true;
    //int zIndex = 1;
    bool screenSpace = false;
    Color modulate = Color::WHITE;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(origin), CEREAL_NVP(shape), CEREAL_NVP(visible), CEREAL_NVP(screenSpace), CEREAL_NVP(modulate));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(origin), CEREAL_NVP(shape), CEREAL_NVP(visible), CEREAL_NVP(screenSpace), CEREAL_NVP(modulate));
    }
};

CEREAL_REGISTER_TYPE(ShapeSprite)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ShapeSprite)

CEREAL_REGISTER_TYPE(BoxShape);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Shape, BoxShape);