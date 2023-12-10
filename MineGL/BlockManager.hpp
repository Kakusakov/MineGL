#pragma once
#include "Glfw.hpp"
#include <vector>
#include <glm/glm.hpp>

enum BlockType {
	blockNotReady = -1,
	Air,
	Bedrock,
	Stone,
	Dirt,
	Grass,
	BlocksTypesCount
};

// TODO: assign as a member of BlockData...
struct TextureBlockData {
public:
	bool isTransparent = false;
	glm::vec2 sideUVs[6] = {};
};

// TODO: assign as a member of BlockData...
struct PhysicsBlockData {
	bool isSolid = true;
};

struct BlockData {
	TextureBlockData tex;
	PhysicsBlockData phy;
};

class BlockManager {
public:
	// TODO: this should be static and const
	std::vector<BlockData> blocks = {};
};
