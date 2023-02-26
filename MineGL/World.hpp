#pragma once
#include "BlockManager.hpp"
#include <map>

const int chunkWidth = 8;
const int chunkHeight = 16;
const int chunkBlockCount = chunkWidth * chunkWidth * chunkHeight;

struct Chunk
{
public:
	glm::ivec2 chunkPos;
	int blocksCache[chunkBlockCount];
	GLuint VAO;
};

class World
{
private:
	BlockManager blockManager;
};

