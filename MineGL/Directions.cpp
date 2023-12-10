#include "Directions.hpp"

glm::ivec3 voxelEdgeOffsets[6][4] = {
		{ {1, 1, 1}, {1, 1, 0}, {0, 1, 0}, {0, 1, 1} },
		{ {0, 0, 1}, {0, 0, 0}, {1, 0, 0}, {1, 0, 1} },
		{ {0, 1, 1}, {0, 0, 1}, {1, 0, 1}, {1, 1, 1} },
		{ {1, 1, 0}, {1, 0, 0}, {0, 0, 0}, {0, 1, 0} },
		{ {0, 1, 0}, {0, 0, 0}, {0, 0, 1}, {0, 1, 1} },
		{ {1, 1, 1}, {1, 0, 1}, {1, 0, 0}, {1, 1, 0} }
};
glm::ivec3 voxelFaceNormals[6] = {
	{ 0, 1, 0 },
	{ 0, -1, 0 },
	{ 0, 0, 1 },
	{ 0, 0, -1 },
	{ -1, 0, 0 },
	{ 1, 0, 0 },
};
glm::vec2 voxelTexCoords[4] = {
	{0.0, 1.0}, {0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}
};