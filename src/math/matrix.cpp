#include "math/matrix.hpp"

namespace drop::math
{

    Mat4 OrthographicProjection(f32 left, f32 right, f32 top, f32 bottom)
    {
        Mat4 matrix {};
        matrix.aw    = -(right + left) / (right - left);
        matrix.bw    = (top + bottom) / (top - bottom);
        matrix.cw    = 0.0f; // Near plane.
        matrix[0][0] = 2.0f / (right - left);
        matrix[1][1] = 2.0f / (top - bottom);
        matrix[2][2] = 1.0f / (1.0f - 0.0f); // Far plane.
        matrix[3][3] = 1.0f;

        return matrix;
    }

} // namespace drop::math
