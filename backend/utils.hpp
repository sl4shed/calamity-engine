#pragma once
#include "definitions.hpp"
#include "components.hpp"

Vector2D addVectors(Vector2D a, Vector2D b);
Vector2D multiplyVectors(Vector2D a, Vector2D b);
Transform combineTransforms(const Transform &parent, const Transform &child);