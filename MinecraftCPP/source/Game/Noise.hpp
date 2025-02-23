#pragma once

#include <vector>
#include <cmath>
#include <random>

class PerlinNoise {
public:

    explicit PerlinNoise(unsigned int seed = 0);

    double noise(double x, double y, double z) const;

private:

    static double fade(double t);
    static double lerp(double t, double a, double b);
    static double grad(int hash, double x, double y, double z);

    std::vector<int> p;
};