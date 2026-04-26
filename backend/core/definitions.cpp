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

constexpr double PI = 3.14159265358979323846;

double Transform::degToRad(const double degrees) {
    return degrees * (PI / 180.0);
}

void Transform::rotate(const float angle)
{
    transformation = Matrix2::rotation(degToRad(angle)) * transformation;
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