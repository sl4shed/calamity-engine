#include "definitions.hpp"
#include <cmath>

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

Matrix2 Matrix2::operator/(float scalar) const
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
    float cos = std::cos(angle);
    float sin = std::sin(angle);

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
// transform /////////////////////////////
///////////////////////////////////////////`

void Transform::rotate(float angle)
{
    transformation = Matrix2::rotation(angle) * transformation;
}

void Transform::scale(Vector2 scale)
{
    transformation = Matrix2::scale(scale) * transformation;
}

float Transform::getAngle()
{
    return std::atan2(transformation.m[1][0], transformation.m[0][0]);
}

float Transform::getDegrees()
{
    return getAngle() * (180.0f / 3.14159265f);
}

Vector2 Transform::getScale()
{
    Vector2 scale;
    scale.x = std::sqrt(transformation.m[0][0] * transformation.m[0][0] +
                        transformation.m[1][0] * transformation.m[1][0]);
    scale.y = std::sqrt(transformation.m[0][1] * transformation.m[0][1] +
                        transformation.m[1][1] * transformation.m[1][1]);
    return scale;
}