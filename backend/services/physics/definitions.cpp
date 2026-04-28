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

CircleShape::CircleShape(const float radius, const Vector2 center, const Vector2 origin)
{
    this->radius = radius;
    this->origin = origin;

    this->circle.center = center;
    this->circle.radius = radius;

    this->shapeDef = b2DefaultShapeDef();
    this->scaledCircle.center = center * PhysicsConstants::scale;
    this->scaledCircle.radius = radius * PhysicsConstants::scale;
}

CapsuleShape::CapsuleShape(const Vector2 center1, const Vector2 center2, const float radius, const Vector2 origin)
{
    capsule.center1 = center1;
    capsule.center2 = center2;
    capsule.radius = radius;
    this->origin = origin;

    scaledCapsule.center1 = center1 * PhysicsConstants::scale;
    scaledCapsule.center2 = center2 * PhysicsConstants::scale;
    scaledCapsule.radius = radius * PhysicsConstants::scale;

    this->shapeDef = b2DefaultShapeDef();
}

// PolygonShape::PolygonShape(Polygon polygon, Vector2 origin)
// {
//     this->origin = origin;
//     this->polygon = polygon;
//     this->shapeDef = b2DefaultShapeDef();
//
//     this->scaledPolygon = polygon;
//     this->scaledPolygon = polygon;
// }

// shape sprite

ShapeSprite::ShapeSprite() = default;
ShapeSprite::ShapeSprite(const std::shared_ptr<Shape>& shape)
{
    this->shape = shape;
}