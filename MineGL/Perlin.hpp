#pragma once
#include <array>
#include <random>
#include <algorithm>
#include <glm/glm.hpp>

class Perlin {
private:
	std::minstd_rand::result_type seed;
    std::array<unsigned char, 256 * 2> p = {};

    inline float fade(float t) const { return t * t * t * (t * (t * 6 - 15) + 10); }
    inline float lerp(float t, float a, float b) const { return a + t * (b - a); }
    inline float grad3(size_t hash, float x, float y, float z) const {
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }
    inline float grad2(size_t hash, float x, float y) const {
        return ((hash & 1) == 0 ? x : -x) + ((hash & 2) == 0 ? y : -y);
    }
    inline float grad1(size_t hash, float x) const {
        return (hash & 1) == 0 ? x : -x;
    }
public:
	Perlin(std::minstd_rand::result_type seed = 0) : seed(seed) {
		for (size_t i = 0; i < 256; i++) p[i] = (unsigned char)i;
		std::shuffle(p.begin(), p.begin() + 256, std::minstd_rand(seed));
		for (size_t i = 0; i < 256; i++) p[i + 256] = p[i];
	}
	inline std::minstd_rand::result_type getSeed() const { return seed; }
    float perlin3(float x, float y, float z) const {
        size_t xi = (size_t)x & 255;
        size_t yi = (size_t)y & 255;
        size_t zi = (size_t)z & 255;
        x -= glm::floor(x);
        y -= glm::floor(y);
        z -= glm::floor(z);
        float u = fade(x);
        float v = fade(y);
        float w = fade(z);
        size_t A = p[xi] + yi;
        size_t AA = p[A] + zi;
        size_t AB = p[A + 1] + zi;
        size_t B = p[xi + 1] + yi;
        size_t BA = p[B] + zi;
        size_t BB = p[B + 1] + zi;

        return lerp(w,
            lerp(v,
                lerp(u, grad3(p[AA], x, y, z), grad3(p[BA], x - 1, y, z)),
                lerp(u, grad3(p[AB], x, y - 1, z), grad3(p[BB], x - 1, y - 1, z))
            ),
            lerp(v,
                lerp(u, grad3(p[AA + 1], x, y, z - 1), grad3(p[BA + 1], x - 1, y, z - 1)),
                lerp(u, grad3(p[AB + 1], x, y - 1, z - 1), grad3(p[BB + 1], x - 1, y - 1, z - 1))
            )
        );
    }
    float perlin2(float x, float y) const {
        size_t xi = (size_t)x & 255;
        size_t yi = (size_t)y & 255;
        x -= glm::floor(x);
        y -= glm::floor(y);
        float u = fade(x);
        float v = fade(y);
        size_t A = p[xi] + yi;
        size_t B = p[xi + 1] + yi;

        return lerp(v,
            lerp(u, grad2(p[A], x, y), grad2(p[B], x - 1, y)),
            lerp(u, grad2(p[A + 1], x, y - 1), grad2(p[B + 1], x - 1, y - 1))
        );
    }
    float perlin1(float x) const {
        size_t xi = (size_t)x & 255;
        x -= glm::floor(x);
        float u = fade(x);

        return lerp(u, grad1(p[xi], x), grad1(p[xi + 1], x - 1));
    }
};

// TODO: this is unnecessary wrapping
class FBM {
public:
    Perlin noiseGenerator = {};
    unsigned ocataveCount = 1;
    float scale = 1.0f;
    float amplitude = 1.0f;
    float lacunarity = 2.0f;
    float persistance = 0.5f;
    float FBM1(float x) const {
        float currentScale = scale;
        float currentAmplitude = amplitude / 2.0f;
        float result = 0.0f;
        for (unsigned i = 0; i < ocataveCount; i++) {
            result += (noiseGenerator.perlin1(x * currentScale) + 1) * currentAmplitude;
            currentScale *= lacunarity;
            currentAmplitude *= persistance;
        }
        return result;
    }
    float FBM2(float x, float y) const {
        float currentScale = scale;
        float currentAmplitude = amplitude / 2.0f;
        float result = 0.0f;
        for (unsigned i = 0; i < ocataveCount; i++) {
            result += (noiseGenerator.perlin2(x * currentScale, y * currentScale) + 1) * currentAmplitude;
            currentScale *= lacunarity;
            currentAmplitude *= persistance;
        }
        return result;
    }
    float FBM3(float x, float y, float z) const {
        float currentScale = scale;
        float currentAmplitude = amplitude / 2.0f;
        float result = 0.0f;
        for (unsigned i = 0; i < ocataveCount; i++) {
            result += (noiseGenerator.perlin3(x * currentScale, y * currentScale, z * currentScale) + 1) * currentAmplitude;
            currentScale *= lacunarity;
            currentAmplitude *= persistance;
        }
        return result;
    }
};

