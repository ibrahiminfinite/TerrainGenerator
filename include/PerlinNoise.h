
#ifndef TERRAINGENERATOR_PERLINNOISE_H
#define TERRAINGENERATOR_PERLINNOISE_H

#include <vector>

class PerlinNoise {

public:

	PerlinNoise();

	float noise(float x, float y, float z);

private:

    std::vector<int> p;

	float fade(float t);
	float lerp(float t, float a, float b);
	float grad(int hash, float x, float y, float z);
};

#endif
