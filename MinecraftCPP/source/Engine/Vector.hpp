#pragma once

#include "../Game/Game.hpp"

Vector3 AddVectors(Vector3 v1, Vector3 v2);

Vector3 SubtractVectors(Vector3 v1, Vector3 v2);

Vector3 DivisionVectors(Vector3 v1, const float coeff);

Vector3 TransformVectors(Vector3 v, Matrix mat);

Matrix MatrixMultiply(Matrix a, Matrix b);

Matrix MatrixRotateXYZ(Vector3 angles);

float DistanceSquared(const Vector3& a, const Vector3& b);