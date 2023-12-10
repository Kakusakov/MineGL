#pragma once
#include <glm/glm.hpp>

// TOOD: this header should be a part of chunk rendering...

enum BlockSide {
	BlockTop,
	BlockBottom,
	BlockFront,
	BlockBack,
	BlockRight,
	BlockLeft
};

extern glm::ivec3 voxelEdgeOffsets[6][4];
extern glm::ivec3 voxelFaceNormals[6];
extern glm::vec2 voxelTexCoords[4];
