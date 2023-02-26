#pragma once
#include "EngineCore.hpp"
#include <vector>
#include <glm/vec2.hpp>
#include <string>

enum BlockSide {
	BlockTop,
	BlockBottom,
	BlockFront,
	BlockBack,
	BlockRight,
	BlockLeft
};

struct BlockData {
public:
	glm::ivec2 sideUVs[6];
};

class BlockManager
{
private:
	std::vector<BlockData> loadedBlocks = {};
public:
	inline size_t getBlocksCount() const { 
		return loadedBlocks.size(); 
	}
	inline const BlockData& getBlock(size_t id) const { 
		return loadedBlocks[id]; 
	}
	inline void LoadBlock(const BlockData& blockData) { 
		loadedBlocks.push_back(blockData); 
	}
};

