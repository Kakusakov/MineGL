#include "TerrainGenerator.hpp"

int TerrainGenerator::generateHeight(const glm::ivec3& worldPos) const {
	return seaLevel + (int)noiseGenerator.FBM2(worldPos.x + 1e6f + 0.5f, worldPos.z + 1e6f + 0.5f);
}
