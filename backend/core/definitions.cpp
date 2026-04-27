#include "definitions.hpp"
#include <cmath>
#include <SDL3/SDL.h>
#include "../services/graphics.hpp"
#include "../services/services.hpp"
#include "../utils/file.hpp"
#include <cereal/archives/json.hpp>
#include <fstream>

////////////////////////////////////////////
// matrix /////////////////////////////////
///////////////////////////////////////////

Matrix2 Matrix2::operator*(const Matrix2 &other) const
{
    Matrix2 result;

    result.m[0][0] = m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0];
    result.m[0][1] = m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1];
    result.m[1][0] = m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0];
    result.m[1][1] = m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1];
    return result;
}

Matrix2 Matrix2::operator+(const Matrix2 &other) const
{
    Matrix2 result;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            result.m[i][j] = m[i][j] + other.m[i][j];
        }
    }
    return result;
}

Matrix2 Matrix2::operator-(const Matrix2 &other) const
{
    Matrix2 result;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            result.m[i][j] = m[i][j] - other.m[i][j];
        }
    }
    return result;
}

Matrix2 Matrix2::operator/(const float scalar) const
{
    Matrix2 result;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            result.m[i][j] = m[i][j] / scalar;
        }
    }
    return result;
}

Vector2 Matrix2::operator*(const Vector2 &other) const
{
    Vector2 result;
    result.x = m[0][0] * other.x + m[0][1] * other.y;
    result.y = m[1][0] * other.x + m[1][1] * other.y;
    return result;
}

// Matrix2 Matrix2::translation(Vector2 t)
// {
//     Matrix2 result;
//     result.m[0][0] = 1.0f;
//     result.m[0][1] = t.x;
//     result.m[1][0] = 0.0f;
//     result.m[1][1] = t.y;
//     return result;
// }

Matrix2 Matrix2::rotation(float angle)
{
    const float cos = std::cos(angle);
    const float sin = std::sin(angle);

    Matrix2 result;
    result.m[0][0] = cos;
    result.m[0][1] = -sin;
    result.m[1][0] = sin;
    result.m[1][1] = cos;
    return result;
}

Matrix2 Matrix2::scale(Vector2 s)
{
    Matrix2 result;
    result.m[0][0] = s.x;
    result.m[0][1] = 0.0f;
    result.m[1][0] = 0.0f;
    result.m[1][1] = s.y;
    return result;
}
///////////////////////////////////////////
// transform //////////////////////////////
///////////////////////////////////////////

constexpr float PI = 3.141592653;

float Transform::degToRad(const float degrees) {
    return degrees * (PI / 180.0);
}

float Transform::radToDeg(const float radians)
{
    return radians * (180.0 / PI);
}

void Transform::rotate(const float angle)
{
    transformation = Matrix2::rotation(degToRad(angle)) * transformation;
}

void Transform::rotateRadians(const float radians)
{
    transformation = Matrix2::rotation(radians) * transformation;
}

void Transform::setScale(const Vector2 scale)
{
    // Preserve current rotation, replace scale entirely
    const float angle = getAngle();
    transformation = Matrix2::rotation(angle) * Matrix2::scale(scale);
}

void Transform::scale(const Vector2 scale)
{
    // Multiply existing scale by s, preserving rotation
    const Vector2 currentScale = getScale();
    const float angle = getAngle();
    transformation = Matrix2::rotation(angle) * Matrix2::scale({currentScale.x * scale.x, currentScale.y * scale.y});
}

float Transform::getAngle() const
{
    return radToDeg(std::atan2(transformation.m[1][0], transformation.m[0][0]));
}

float Transform::getAngleRadians() const
{
    return std::atan2(transformation.m[1][0], transformation.m[0][0]);
}

float Transform::getDegrees() const
{
    return getAngle() * (180.0f / PI);
}

void Transform::setAngle(const float angle) {
    const Vector2 currentScale = getScale();
    transformation = Matrix2::rotation(degToRad(angle)) * Matrix2::scale(currentScale);
}

void Transform::setAngleRadians(const float radians)
{
    const Vector2 currentScale = getScale();
    transformation = Matrix2::rotation(radians) * Matrix2::scale(currentScale);
}

Vector2 Transform::getScale() const
{
    Vector2 scale;
    scale.x = std::sqrt(transformation.m[0][0] * transformation.m[0][0] +
                        transformation.m[1][0] * transformation.m[1][0]);
    scale.y = std::sqrt(transformation.m[0][1] * transformation.m[0][1] +
                        transformation.m[1][1] * transformation.m[1][1]);
    return scale;
}

Vector2 Transform::applyTo(const Vector2 &point) const
{
    return transformation * point + position;
}

Transform Transform::applyTo(const Transform &other) const
{
    Transform result;
    result.transformation = transformation * other.transformation;
    result.position = position + (transformation * other.position);
    return result;
}

Transform Transform::inverse() const
{
    Transform result;
    // Inverse of rotation/scale matrix is its transpose divided by determinant
    const float det = transformation.m[0][0] * transformation.m[1][1] - transformation.m[0][1] * transformation.m[1][0];
    if (det == 0)
    {
        // Handle non-invertible case (could throw an error or return identity)
        return Transform();
    }
    Matrix2 invTrans;
    invTrans.m[0][0] = transformation.m[1][1] / det;
    invTrans.m[0][1] = -transformation.m[0][1] / det;
    invTrans.m[1][0] = -transformation.m[1][0] / det;
    invTrans.m[1][1] = transformation.m[0][0] / det;
    result.transformation = invTrans;
    result.position = invTrans * (Vector2{-position.x, -position.y});
    return result;
}

// texture
Texture::Texture(const std::string& _path) : handle(nullptr), width(0), height(0), path(_path)
{
    initialize();
}

void Texture::initialize()
{
    this->handle = Services::graphics()->loadTexture(File::getAbsoluteFilePath(this->path));

    this->width = handle->w;
    this->height = handle->h;
    this->textureWidth = handle->w;
    this->textureHeight = handle->h;
}

Texture::~Texture()
{
    SDL_DestroyTexture(handle);
}

// polygon

Polygon::operator b2Polygon() const {
    b2Polygon polygon;
    polygon.centroid = centroid;
    polygon.count = count;
    for (int i = 0; i < count; i++) {
        polygon.normals[i] = normals[i];
        polygon.vertices[i] = vertices[i];
    }
    polygon.radius = radius;
    return polygon;
}

Polygon::Polygon(const b2Polygon &p) {
    centroid = Vector2(p.centroid);
    count = p.count;
    radius = p.radius;
    for (int i = 0; i < count; i++) {
        normals[i] = Vector2(p.normals[i]);
        vertices[i] = Vector2(p.vertices[i]);
    }
}

Polygon::Polygon() {
    centroid = {0, 0};
    count = 0;
    radius = 0;
    for (int i = 0; i < B2_MAX_POLYGON_VERTICES; i++) {
        normals[i] = {0, 0};
        vertices[i] = {0, 0};
    }
}

// circle

Circle::Circle()
{
    center = {0.5f, 0.5f};
    radius = 0;
}

Circle::Circle(const b2Circle &circle)
{
    center = Vector2(circle.center);
    radius = circle.radius;
}

Circle::operator b2Circle() const
{
    b2Circle circle;
    circle.radius = radius;
    circle.center = center;
    return circle;
}

// capsule

Capsule::Capsule()
{
    radius = 0;
    center1 = {0.0f, 0.0f};
    center2 = {0.0f, 0.0f};
}

Capsule::Capsule(const b2Capsule &capsule)
{
    radius = capsule.radius;
    center1 = Vector2(capsule.center1);
    center2 = Vector2(capsule.center2);
}

Capsule::operator b2Capsule() const
{
    b2Capsule capsule;
    capsule.radius = radius;
    capsule.center1 = center1;
    capsule.center2 = center2;
    return capsule;
}