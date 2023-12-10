#pragma once
#include "BlockManager.hpp"
#include "TextureManager.hpp"
#include "TerrainGenerator.hpp"
#include "Player.hpp"
#include "Chunk.hpp"

#include <glm/gtx/hash.hpp>

#include <unordered_map>
#include <string>
#include <memory>

const std::string chunkDirName = ".chunk\\";
const std::string worldDirName = "Worlds\\";

struct Raycast {
public:
	bool hasCollided = false;
	float magnitude = 0.0f;
	glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0);
	glm::ivec3 worldPos = glm::ivec3(0, 0, 0);
	glm::ivec3 normal = glm::ivec3(0, 1, 0);
};

class World {
private:
	TextureManager textureManager = {};
	BlockManager blockManager = {};
	TerrainGenerator terraGen = {};
	// maybe turn this into an object to avoid two-way references with entities
	// (add physics related methods to this object)
	std::unordered_map<glm::ivec2, Chunk> chunkMap = {};
	Player player = {};
	std::string worldName = "New World";

	void draw(GLuint chunkProg, GLuint voxelProg);

	// chunk
	Chunk generateChunk(const glm::ivec2& chunkPos);
	bool tryLoadChunk(const glm::ivec2& chunkPos, Chunk& chunk);
	bool tryStoreChunk(const Chunk& chunk);
	void rebuildChunkMesh(Chunk& ch);
	void rebuildAdjacentChunkMeshes(glm::ivec2 chunkPos);
	void updateChunks();

	// player
	void drawPlayerUI(GLuint prog) const;
	void drawPlayerLookHighlight(
		GLuint prog,
		const glm::mat4& view,
		const glm::mat4& projection
	) const;
	void movePlayerAsFreeCam();
	void movePlayerAsCollider();
	void updatePlayerInput();
public:
	// @throws std::runtime_error
	// @throws std::bad_alloc
	// @throws filesystem::filesystem_error
	World(
		BlockManager blockManager,
		TextureManager textureManager,
		TerrainGenerator terrainGenerator,
		std::string worldName
	);
	~World();
	void update(GLuint chunkProg, GLuint voxelProg) {
		updatePlayerInput();
		updateChunks();
		draw(chunkProg, voxelProg);
	}
	inline const Player& getPlayer() { return player; }
	inline void teleportPlayer(const glm::vec3& pos) { player.camera.pos = pos; }
	inline int getBlock(const glm::ivec3& worldPos) const {
		if (worldPos.y < 0 || worldPos.y >= Chunk::Height) return Air;
		auto chunkIt = chunkMap.find(toChunkPos(worldPos));
		if (chunkIt == chunkMap.end()) return blockNotReady;
		return chunkIt->second.blocksCache[toBlockIndex(toChunkLocal(worldPos))];
	}
	inline bool pointCast(const glm::ivec3& worldPos) const {
		int block = getBlock(worldPos);
		return block == blockNotReady || blockManager.blocks[block].phy.isSolid;
	}
	inline bool boxCast(const glm::ivec3& min, const glm::ivec3& max) const {
		glm::ivec3 pos = min;
		for (pos.x = min.x; pos.x <= max.x; pos.x++) {
			for (pos.y = min.y; pos.y <= max.y; pos.y++) {
				for (pos.z = min.z; pos.z <= max.z; pos.z++) {
					// Using point cast is inefficient (has to find chunk every time),
					// but it will do for now.
					if (pointCast(pos)) return true;
				}
			}
		}
		return false;
	}
	Raycast rayCast(
		const glm::vec3& pos,
		const glm::vec3& dir,
		float maxDistance
	) const;
};
