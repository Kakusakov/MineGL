#pragma once
#include <glm/glm.hpp>

enum class ChunkRenderDistance {
	Short4 = 4,
	Average8 = 8,
	Huge16 = 16,
	Insane32 = 32,
	Ridiculous64 = 64
};

// this should probably be split into world local and global settings.
class GlobalSettings {
private:
	ChunkRenderDistance chunkRenderDistance = ChunkRenderDistance::Average8;
public:
	glm::vec4 skyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	float playerLookDistance = 5.5f;
	float playerFreeCamSpeed = 50.0f;
	float playerColliderSpeed = 8.0f;
	float playerJumpSpeed = 7.0f;

	float mouseSpeed = 0.05f;

	inline int getChunkRenderDistance() const {
		return (int)chunkRenderDistance;
	}
	inline void setChunkRenderDistance(ChunkRenderDistance value) {
		chunkRenderDistance = value;
	}
};

extern GlobalSettings globalSettings;
