#include "Vector.hpp"

Vector3 AddVectors(Vector3 v1, Vector3 v2) {
    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

Vector3 SubtractVectors(Vector3 v1, Vector3 v2) {
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

Vector3 DivisionVectors(Vector3 v1, const float coeff) {
    return { v1.x / coeff, v1.y / coeff, v1.z / coeff };
}

Vector3 NormalizeVector(const Vector3& v) {
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0.0f) return { 0, 0, 0 };
    return { v.x / length, v.y / length, v.z / length };
}

Vector3 ScaleVector(const Vector3& v, float scale) {
    return { v.x * scale, v.y * scale, v.z * scale };
}

Vector3 TransformVectors(Vector3 v, Matrix mat) {
    Vector3 result = { 0 };

    result.x = v.x * mat.m0 + v.y * mat.m4 + v.z * mat.m8 + mat.m12;
    result.y = v.x * mat.m1 + v.y * mat.m5 + v.z * mat.m9 + mat.m13;
    result.z = v.x * mat.m2 + v.y * mat.m6 + v.z * mat.m10 + mat.m14;

    return result;
}

Matrix MatrixMultiply(Matrix a, Matrix b) {

    Matrix result = { 0 };

    result.m0 = a.m0 * b.m0 + a.m4 * b.m1 + a.m8 * b.m2 + a.m12 * b.m3;
    result.m1 = a.m1 * b.m0 + a.m5 * b.m1 + a.m9 * b.m2 + a.m13 * b.m3;
    result.m2 = a.m2 * b.m0 + a.m6 * b.m1 + a.m10 * b.m2 + a.m14 * b.m3;
    result.m3 = a.m3 * b.m0 + a.m7 * b.m1 + a.m11 * b.m2 + a.m15 * b.m3;

    result.m4 = a.m0 * b.m4 + a.m4 * b.m5 + a.m8 * b.m6 + a.m12 * b.m7;
    result.m5 = a.m1 * b.m4 + a.m5 * b.m5 + a.m9 * b.m6 + a.m13 * b.m7;
    result.m6 = a.m2 * b.m4 + a.m6 * b.m5 + a.m10 * b.m6 + a.m14 * b.m7;
    result.m7 = a.m3 * b.m4 + a.m7 * b.m5 + a.m11 * b.m6 + a.m15 * b.m7;

    result.m8 = a.m0 * b.m8 + a.m4 * b.m9 + a.m8 * b.m10 + a.m12 * b.m11;
    result.m9 = a.m1 * b.m8 + a.m5 * b.m9 + a.m9 * b.m10 + a.m13 * b.m11;
    result.m10 = a.m2 * b.m8 + a.m6 * b.m9 + a.m10 * b.m10 + a.m14 * b.m11;
    result.m11 = a.m3 * b.m8 + a.m7 * b.m9 + a.m11 * b.m10 + a.m15 * b.m11;

    result.m12 = a.m0 * b.m12 + a.m4 * b.m13 + a.m8 * b.m14 + a.m12 * b.m15;
    result.m13 = a.m1 * b.m12 + a.m5 * b.m13 + a.m9 * b.m14 + a.m13 * b.m15;
    result.m14 = a.m2 * b.m12 + a.m6 * b.m13 + a.m10 * b.m14 + a.m14 * b.m15;
    result.m15 = a.m3 * b.m12 + a.m7 * b.m13 + a.m11 * b.m14 + a.m15 * b.m15;

    return result;
}



Matrix MatrixRotateXYZ(Vector3 angles) {

    float cosX = cosf(angles.x);
    float sinX = sinf(angles.x);
    float cosY = cosf(angles.y);
    float sinY = sinf(angles.y);
    float cosZ = cosf(angles.z);
    float sinZ = sinf(angles.z);

    Matrix rotX = {
        1,    0,     0,    0,
        0,  cosX, -sinX,   0,
        0,  sinX,  cosX,   0,
        0,    0,     0,    1
    };

    Matrix rotY = {
        cosY,  0,  sinY,   0,
          0,   1,    0,    0,
       -sinY,  0,  cosY,   0,
          0,   0,    0,    1
    };

    Matrix rotZ = {
        cosZ, -sinZ,  0,   0,
        sinZ,  cosZ,  0,   0,
         0,      0,   1,   0,
         0,      0,   0,   1
    };

    Matrix result = MatrixMultiply(rotZ, MatrixMultiply(rotY, rotX));

    return result;
}

float DistanceSquared(const Vector3& a, const Vector3& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

Vector4 Vector4Transform(const Vector4& v, const Matrix& mat) {
    return {
        v.x * mat.m0 + v.y * mat.m4 + v.z * mat.m8 + v.w * mat.m12,
        v.x * mat.m1 + v.y * mat.m5 + v.z * mat.m9 + v.w * mat.m13,
        v.x * mat.m2 + v.y * mat.m6 + v.z * mat.m10 + v.w * mat.m14,
        v.x * mat.m3 + v.y * mat.m7 + v.z * mat.m11 + v.w * mat.m15
    };
}

Matrix MatrixPerspective(float fovy, float aspect, float nearPlane, float farPlane) {
    float f = 1.0f / tanf(fovy / 2.0f);
    Matrix result = { 0 };

    result.m0 = f / aspect;
    result.m5 = f;
    result.m10 = (farPlane + nearPlane) / (nearPlane - farPlane);
    result.m11 = -1.0f;
    result.m14 = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);

    return result;
}



Matrix GetCameraProjection(const Camera3D& camera) {
    float aspect = (float)GetScreenWidth() / (float)GetScreenHeight();
    return MatrixPerspective(camera.fovy * DEG2RAD, aspect, 0.01f, 1000.0f);
}
