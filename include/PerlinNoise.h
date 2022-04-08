
#include <vector>

#ifndef TERRAINGENERATOR_PERLINNOISE_H
#define TERRAINGENERATOR_PERLINNOISE_H

class PerlinNoise {
	std::vector<int> p;

public:

	PerlinNoise();

	float noise(float x, float y, float z);

private:
	float fade(float t);
	float lerp(float t, float a, float b);
	float grad(int hash, float x, float y, float z);
};

#endif
