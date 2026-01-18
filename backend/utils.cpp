#include "utils.hpp"
#include <cmath>

Transform addTransforms(Transform parent, Transform child) {
    Transform result;
    result.angle = parent.angle + child.angle;
    result.scale = multiplyVectors(parent.scale, child.scale);

    // black magic
    float radians = parent.angle * 3.141592653f / 180.0f;
    float cos_a = cos(radians);
    float sin_a = sin(radians);

    Vector2D offsetPos;
    offsetPos.x = child.position.x - parent.origin.x;
    offsetPos.y = child.position.y - parent.origin.y;

    Vector2D rotatedPos;
    rotatedPos.x = offsetPos.x * cos_a - offsetPos.y * sin_a;
    rotatedPos.y = offsetPos.x * sin_a + offsetPos.y * cos_a;

    result.position.x = parent.position.x + rotatedPos.x + parent.origin.x;
    result.position.y = parent.position.y + rotatedPos.y + parent.origin.y;
    result.origin = child.origin;

    return result;
}

Vector2D multiplyVectors(Vector2D a, Vector2D b) {
    Vector2D result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

Vector2D addVectors(Vector2D a, Vector2D b) {
    Vector2D result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}
