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