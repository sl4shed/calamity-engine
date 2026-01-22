#include "definitions.hpp"
#include <cmath>

////////////////////////////////////////////
// matrix /////////////////////////////////
///////////////////////////////////////////

Matrix3 Matrix3::operator*(const Matrix3 &other) const
{
    Matrix3 result;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            result.m[i][j] = 0.0f;
            for (int k = 0; k < 3; k++)
            {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

Matrix3 Matrix3::operator+(const Matrix3 &other) const
{
    Matrix3 result;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            result.m[i][j] = m[i][j] + other.m[i][j];
        }
    }
    return result;
}

Matrix3 Matrix3::operator-(const Matrix3 &other) const
{
    Matrix3 result;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            result.m[i][j] = m[i][j] - other.m[i][j];
        }
    }
    return result;
}

Matrix3 Matrix3::operator/(float scalar) const
{
    Matrix3 result;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            result.m[i][j] = m[i][j] / scalar;
        }
    }
    return result;
}

Matrix3 Matrix3::translation(Vector2 t)
{
    Matrix3 result;
    result.m[0][0] = 1.0f;
    result.m[0][1] = 0.0f;
    result.m[0][2] = t.x;
    result.m[1][0] = 0.0f;
    result.m[1][1] = 1.0f;
    result.m[1][2] = t.y;
    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = 1.0f;
    return result;
}

Matrix3 Matrix3::rotation(float angle)
{
    float cos = std::cos(angle);
    float sin = std::sin(angle);

    Matrix3 result;
    result.m[0][0] = cos;
    result.m[0][1] = -sin;
    result.m[0][2] = 0.0f;
    result.m[1][0] = sin;
    result.m[1][1] = cos;
    result.m[1][2] = 0.0f;
    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = 1.0f;
    return result;
}

Matrix3 Matrix3::scale(Vector2 s)
{
    Matrix3 result;
    result.m[0][0] = s.x;
    result.m[0][1] = 0.0f;
    result.m[0][2] = 0.0f;
    result.m[1][0] = 0.0f;
    result.m[1][1] = s.y;
    result.m[1][2] = 0.0f;
    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = 1.0f;
    return result;
}