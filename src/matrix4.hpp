#ifndef MATRIX4_HPP
#define MATRIX4_HPP

#include <cmath>
#include <iostream>
#include <stdexcept>

// Forward declarations to integrate seamlessly with your project's vector classes
#include "vector.hpp"

class matrix4
{
public:
    double m[4][4];

    // Default constructor: creates a zero matrix
    matrix4()
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = 0.0;
    }

    // Creates an identity matrix
    static matrix4 identity()
    {
        matrix4 mat;
        mat.m[0][0] = 1.0;
        mat.m[1][1] = 1.0;
        mat.m[2][2] = 1.0;
        mat.m[3][3] = 1.0;
        return mat;
    }

    // Multiplies two 4x4 matrices together (Matrix Cascading)
    matrix4 operator*(const matrix4 &other) const
    {
        matrix4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.m[i][j] = m[i][0] * other.m[0][j] +
                                 m[i][1] * other.m[1][j] +
                                 m[i][2] * other.m[2][j] +
                                 m[i][3] * other.m[3][j];
            }
        }
        return result;
    }

    // Transform a Point (w = 1). Applies translation, rotation, and scaling.
    point3 transform_point(const point3 &p) const
    {
        double x = m[0][0] * p.x() + m[0][1] * p.y() + m[0][2] * p.z() + m[0][3];
        double y = m[1][0] * p.x() + m[1][1] * p.y() + m[1][2] * p.z() + m[1][3];
        double z = m[2][0] * p.x() + m[2][1] * p.y() + m[2][2] * p.z() + m[2][3];
        double w = m[3][0] * p.x() + m[3][1] * p.y() + m[3][2] * p.z() + m[3][3];

        if (w != 1.0 && w != 0.0)
        {
            return point3(x / w, y / w, z / w);
        }
        return point3(x, y, z);
    }

    // Transform a Vector (w = 0). Ignores translation, applies only rotation and scaling.
    vec3 transform_vector(const vec3 &v) const
    {
        double x = m[0][0] * v.x() + m[0][1] * v.y() + m[0][2] * v.z();
        double y = m[1][0] * v.x() + m[1][1] * v.y() + m[1][2] * v.z();
        double z = m[2][0] * v.x() + m[2][1] * v.y() + m[2][2] * v.z();
        return vec3(x, y, z);
    }

    // Transpose matrix (Useful for transforming normals using the inverse transpose)
    matrix4 transpose() const
    {
        matrix4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.m[i][j] = m[j][i];
            }
        }
        return result;
    }

    // --- Transformation Matrix Builders ---

    static matrix4 translation(const vec3 &offset)
    {
        matrix4 mat = identity();
        mat.m[0][3] = offset.x();
        mat.m[1][3] = offset.y();
        mat.m[2][3] = offset.z();
        return mat;
    }

    static matrix4 scaling(const vec3 &s)
    {
        matrix4 mat = identity();
        mat.m[0][0] = s.x();
        mat.m[1][1] = s.y();
        mat.m[2][2] = s.z();
        return mat;
    }

    static matrix4 rotation_y(double degrees)
    {
        matrix4 mat = identity();
        double radians = degrees * M_PI / 180.0;
        double sin_t = std::sin(radians);
        double cos_t = std::cos(radians);

        mat.m[0][0] = cos_t;
        mat.m[0][2] = sin_t;
        mat.m[2][0] = -sin_t;
        mat.m[2][2] = cos_t;
        return mat;
    }

    static matrix4 rotation_x(double degrees)
    {
        matrix4 mat = identity();
        double radians = degrees * M_PI / 180.0;
        double sin_t = std::sin(radians);
        double cos_t = std::cos(radians);

        mat.m[1][1] = cos_t;
        mat.m[1][2] = -sin_t;
        mat.m[2][1] = sin_t;
        mat.m[2][2] = cos_t;
        return mat;
    }

    static matrix4 rotation_z(double degrees)
    {
        matrix4 mat = identity();
        double radians = degrees * M_PI / 180.0;
        double sin_t = std::sin(radians);
        double cos_t = std::cos(radians);

        mat.m[0][0] = cos_t;
        mat.m[0][1] = -sin_t;
        mat.m[1][0] = sin_t;
        mat.m[1][1] = cos_t;
        return mat;
    }

    // --- Matrix Inversion (Gauss-Jordan Elimination) ---
    // Necessary to bring the ray from World Space down into Object Local Space
    matrix4 inverse() const
    {
        double temp[4][8];

        // Setup augmented matrix [ Matrix | Identity ]
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                temp[i][j] = m[i][j];
                temp[i][j + 4] = (i == j) ? 1.0 : 0.0;
            }
        }

        // Gauss-Jordan elimination
        for (int i = 0; i < 4; ++i)
        {
            // Pivot selection
            double pivot = temp[i][i];
            if (std::abs(pivot) < 1e-9)
            {
                // If a non-invertible scaling (like 0) occurs, return identity safely
                return identity();
            }

            // Normalize row
            for (int j = 0; j < 8; ++j)
            {
                temp[i][j] /= pivot;
            }

            // Eliminate column values in other rows
            for (int k = 0; k < 4; ++k)
            {
                if (k != i)
                {
                    double factor = temp[k][i];
                    for (int j = 0; j < 8; ++j)
                    {
                        temp[k][j] -= factor * temp[i][j];
                    }
                }
            }
        }

        // Extract the right side (the inverted matrix)
        matrix4 inv;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                inv.m[i][j] = temp[i][j + 4];
            }
        }
        return inv;
    }
};

#endif