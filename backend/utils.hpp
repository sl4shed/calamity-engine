#pragma once
#include "definitions.hpp"
#include "components.hpp"

Vector2 addVectors(Vector2 a, Vector2 b);
Vector2 multiplyVectors(Vector2 a, Vector2 b);
Transform combineTransforms(const Transform &parent, const Transform &child);