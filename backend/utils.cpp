#include "utils.hpp"
#include <cmath>
#include "node.hpp"
#include "definitions.hpp"

Vector2 multiplyVectors(Vector2 a, Vector2 b)
{
    Vector2 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

Vector2 addVectors(Vector2 a, Vector2 b)
{
    Vector2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

Transform combineTransforms(const Transform &parent, const Transform &child)
{
    // black magic
    Transform result;
    result.angle = parent.angle + child.angle;
    result.scale = child.scale;
    result.origin = child.origin;

    float rad = parent.angle * 3.14159265f / 180.0f;
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);

    Vector2 parent_origin_pixels = {
        parent.origin.x * parent.scale.x,
        parent.origin.y * parent.scale.y};

    Vector2 child_relative = {
        child.position.x - parent_origin_pixels.x,
        child.position.y - parent_origin_pixels.y};

    Vector2 rotated;
    rotated.x = child_relative.x * cos_a - child_relative.y * sin_a;
    rotated.y = child_relative.x * sin_a + child_relative.y * cos_a;

    result.position.x = parent.position.x + rotated.x + parent_origin_pixels.x;
    result.position.y = parent.position.y + rotated.y + parent_origin_pixels.y;

    return result;
}