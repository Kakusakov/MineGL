#pragma once
#include "Glfw.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/integer.hpp>

#include <string>
#include <vector>

struct Chunk {
	static constexpr size_t Width = 8;
	static constexpr size_t Height = 256;

	std::vector<unsigned char> blocksCache = std::vector<unsigned char>(Chunk::Height * Chunk::Width * Chunk::Width);
	glm::ivec2 chunkPos = glm::ivec2(0, 0);  // is this even needed here?
	// maybe combine vao and vertCount into an object?
	GLuint VAO = 0;
	size_t vertCount = 0;
};

inline glm::ivec2 toChunkPos(const glm::vec3& pos) {
	return glm::ivec2(
		glm::floor(pos.x / Chunk::Width),
		glm::floor(pos.z / Chunk::Width)
	);
}

inline glm::ivec3 toChunkMin(const glm::ivec2& chunkPos) {
	return glm::ivec3(
		chunkPos.x * Chunk::Width,
		0,
		chunkPos.y * Chunk::Width
	);
}

inline glm::ivec3 toIntPos(const glm::vec3& pos) {
	return glm::ivec3(glm::floor(pos));
}

inline glm::uvec3 toChunkLocal(const glm::ivec3& worldPos) {
	return glm::uvec3(
		glm::mod(worldPos.x, Chunk::Width),
		glm::clamp(worldPos.y, 0, (int)Chunk::Height - 1),
		glm::mod(worldPos.z, Chunk::Width)
	);
}

inline glm::uvec3 toChunkLocal(unsigned blockPos) {
	glm::uvec3 localPos = {};
	localPos.z = blockPos % Chunk::Width;
	localPos.x = (blockPos /= Chunk::Width) % Chunk::Width;
	localPos.y = (blockPos /= Chunk::Width);
	return localPos;
}

inline bool isChunkLocal(const glm::vec3& worldPos) {
	return worldPos.x >= 0
		&& worldPos.z >= 0
		&& worldPos.y >= 0
		&& worldPos.x < Chunk::Width
		&& worldPos.z < Chunk::Width
		&& worldPos.y < Chunk::Height;
}

inline size_t toBlockIndex(const glm::uvec3& localPos) {
	return (size_t)localPos.y * Chunk::Width * Chunk::Width
		+ (size_t)localPos.x * Chunk::Width
		+ (size_t)localPos.z;
}

inline std::string getChunkFilename(const glm::ivec2& chunkPos) {
	return "x" + std::to_string(chunkPos.x)
		+ " y" + std::to_string(chunkPos.y)
		+ ".chunk";
}

// TODO: this format is bad, shuld compress to human readable format.
// TODO: assign as member of Chunk.
std::vector<unsigned char> compressChunk(const Chunk& chunk);
bool tryDecompressChunk(const std::vector<unsigned char>& data, Chunk& chunk);
