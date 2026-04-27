#include "definitions.hpp"
#include "../../core/definitions.hpp"

BoxShape::BoxShape(const Vector2 size, const Vector2 center) {
    this->size = size;
    this->origin = center;

    const Vector2 calculatedCenter = origin * size;
    const b2Rot rotation = b2Rot_identity;

    const b2Polygon poly = b2MakeOffsetBox(size.x / 2 * PhysicsConstants::scale, size.y / 2 * PhysicsConstants::scale, calculatedCenter * PhysicsConstants::scale, rotation);
    const b2Polygon polyUnscaled = b2MakeOffsetBox(size.x / 2, size.y / 2, calculatedCenter, rotation);

    this->polygon = static_cast<Polygon>(polyUnscaled);
    this->shapeDef = b2DefaultShapeDef();
    this->scaledPolygon = static_cast<Polygon>(poly);
}

CircleShape::CircleShape(const float radius, const Vector2 origin)
{
    this->radius = radius;
    this->origin = origin;

    const Vector2 calculatedCenter = origin * Vector2{radius * 2, radius * 2};

    this->circle.center = calculatedCenter;
    this->circle.radius = radius;

    this->shapeDef = b2DefaultShapeDef();
    this->scaledCircle.center = calculatedCenter * PhysicsConstants::scale;
    this->scaledCircle.radius = radius * PhysicsConstants::scale;
}

// shape sprite

ShapeSprite::ShapeSprite() = default;
ShapeSprite::ShapeSprite(const std::shared_ptr<Shape>& shape)
{
    this->shape = shape;
}