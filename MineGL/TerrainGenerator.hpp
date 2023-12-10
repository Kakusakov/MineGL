#pragma once
#include "Perlin.hpp"
#include "BlockManager.hpp"

class TerrainGenerator {
public:
	FBM noiseGenerator = {};
	int seaLevel = 1;

	int generateHeight(const glm::ivec3& worldPos) const;

	inline int generateBlock(const glm::ivec3& worldPos) {
		int terrainHeight = generateHeight(worldPos);
		if (worldPos.y == 0) return BlockType::Bedrock;
		else if (worldPos.y < terrainHeight - 2) return BlockType::Stone;
		else if (worldPos.y < terrainHeight) return BlockType::Dirt;
		else if (worldPos.y == terrainHeight) return BlockType::Grass;
		else return BlockType::Air;
	}
};
