#include "utils.hpp"
#include <cmath>
#include "node.hpp"
#include "definitions.hpp"

Vector2D multiplyVectors(Vector2D a, Vector2D b)
{
    Vector2D result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

Vector2D addVectors(Vector2D a, Vector2D b)
{
    Vector2D result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

Transform combineTransforms(const Transform &parent, const Transform &child)
{
    Transform result;

    // Combine angles
    result.angle = parent.angle + child.angle;

    // Child keeps its pixel size (NOT multiplied!)
    result.scale = child.scale;

    // Child keeps its origin multiplier
    result.origin = child.origin;

    // Convert parent angle to radians
    float rad = parent.angle * 3.14159265f / 180.0f;
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);

    // Get parent's origin in pixels
    Vector2D parent_origin_pixels = {
        parent.origin.x * parent.scale.x,
        parent.origin.y * parent.scale.y};

    // Make child position relative to parent's origin
    Vector2D child_relative = {
        child.position.x - parent_origin_pixels.x,
        child.position.y - parent_origin_pixels.y};

    // Apply parent's rotation
    Vector2D rotated;
    rotated.x = child_relative.x * cos_a - child_relative.y * sin_a;
    rotated.y = child_relative.x * sin_a + child_relative.y * cos_a;

    // Add parent position and origin back
    result.position.x = parent.position.x + rotated.x + parent_origin_pixels.x;
    result.position.y = parent.position.y + rotated.y + parent_origin_pixels.y;

    return result;
}