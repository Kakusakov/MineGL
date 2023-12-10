#include "World.hpp"
#include "Settings.hpp"
#include "Timer.hpp"
#include "VoxelMesh.hpp"
#include "Window.hpp"
#include "Directions.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/intersect.hpp>

#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <iostream>

World::World(
	BlockManager blockManager,
	TextureManager textureManager,
	TerrainGenerator terrainGenerator,
	std::string worldName
) {
	this->blockManager = blockManager;
	this->textureManager = textureManager;
	this->terraGen = terrainGenerator;
	this->worldName = worldName;
	if (!std::filesystem::exists(rootDirName + worldDirName + worldName)) {
		std::filesystem::create_directory(rootDirName + worldDirName + worldName);
		std::filesystem::create_directory(rootDirName + worldDirName + worldName + '\\' + chunkDirName);
	}
	else if (std::filesystem::is_directory(rootDirName + worldDirName + worldName)) {
		throw std::runtime_error("world root is not a directory");
	}
}

World::~World() {
	for (const auto& p : chunkMap) {
		glDeleteVertexArrays(1, &p.second.VAO);
		if (!tryStoreChunk(p.second)) {
			std::cerr << "\n[IO ERROR] Failed to store chunk"
				" when destroying World instance\n";
		}
	}
}

Chunk World::generateChunk(const glm::ivec2& chunkPos) {
	glm::ivec3 v = {};
	Chunk ch = {};
	ch.chunkPos = chunkPos;
	glm::ivec3 chunkMin = toChunkMin(chunkPos);

	int i = 0;
	for (v.y = 0; v.y < Chunk::Height; v.y++) {
		int j = 0;
		for (v.x = 0; v.x < Chunk::Width; v.x++) {
			for (v.z = 0; v.z < Chunk::Width; v.z++, i++, j++) {
				ch.blocksCache[i] = terraGen.generateBlock(v + chunkMin);
			}
		}
	}
	return ch;
}

void World::rebuildChunkMesh(Chunk& ch) {
	glm::vec2 blockSize = textureManager.getBlockSize();
	glm::ivec3 chunkMin = toChunkMin(ch.chunkPos);
	ch.vertCount = 0;
	if (ch.VAO != 0) glDeleteVertexArrays(1, &ch.VAO);
	
	Mesh3 mesh = {};
	int i = 0;
	
	for (glm::ivec3 v(0); v.y < Chunk::Height; v.y++) {
		for (v.x = 0; v.x < Chunk::Width; v.x++) {
			for (v.z = 0; v.z < Chunk::Width; v.z++, i++) {
				int block = ch.blocksCache[i];
				if (block == BlockType::Air) continue;
				for (int side = 0; side < 6; side++) {
					glm::ivec3 adj = v + voxelFaceNormals[side];
					if (!isChunkLocal(adj)) {
						int adjBlock = getBlock(chunkMin + adj);
						if (adjBlock == blockNotReady
							|| !blockManager.blocks[adjBlock].tex.isTransparent
						) {
							continue;
						}
					}
					else if (!blockManager.blocks[ch.blocksCache[toBlockIndex(adj)]].tex.isTransparent) {
						continue;
					}
					glm::vec2 uvMin = blockManager.blocks[block].tex.sideUVs[side];
					mesh.vertices.push_back({
						v + voxelEdgeOffsets[side][0],
						{ uvMin + blockSize * voxelTexCoords[0] }
					});
					mesh.vertices.push_back({
						v + voxelEdgeOffsets[side][1],
						{ uvMin + blockSize * voxelTexCoords[1] }
					});
					mesh.vertices.push_back({
						v + voxelEdgeOffsets[side][2],
						{ uvMin + blockSize * voxelTexCoords[2] }
					});
					mesh.vertices.push_back({
						v + voxelEdgeOffsets[side][3],
						{ uvMin + blockSize * voxelTexCoords[3] }
					});
					int lastInd = mesh.indices.empty() ? 0 : mesh.indices.back() + 1;
					mesh.indices.push_back(lastInd);
					mesh.indices.push_back(lastInd + 1);
					mesh.indices.push_back(lastInd + 3);
					mesh.indices.push_back(lastInd + 1);
					mesh.indices.push_back(lastInd + 2);
					mesh.indices.push_back(lastInd + 3);
				}
			}
		}
	}
	ch.vertCount = mesh.indices.size();
	ch.VAO = mesh.buildIndexedVAO();
}

void World::rebuildAdjacentChunkMeshes(glm::ivec2 chunkPos) {
	auto it = chunkMap.find(chunkPos + glm::ivec2(0, 1));
	if (it != chunkMap.end()) rebuildChunkMesh(it->second);
	it = chunkMap.find(chunkPos + glm::ivec2(0, -1));
	if (it != chunkMap.end()) rebuildChunkMesh(it->second);
	it = chunkMap.find(chunkPos + glm::ivec2(1, 0));
	if (it != chunkMap.end()) rebuildChunkMesh(it->second);
	it = chunkMap.find(chunkPos + glm::ivec2(-1, 0));
	if (it != chunkMap.end()) rebuildChunkMesh(it->second);
}

void World::draw(GLuint chunkShader, GLuint voxelShader) {
	glBindTexture(GL_TEXTURE_2D, textureManager.atlasTexture);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glUseProgram(chunkShader);

	GLint location = glGetUniformLocation(chunkShader, "view");
	glm::mat4 view = glm::lookAt(
		player.camera.pos, 
		player.camera.pos + player.camera.getDir(),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));

	location = glGetUniformLocation(chunkShader, "projection");
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);  // is this good?
	glm::mat4 projection = glm::perspective(
		glm::radians(85.0f),
		((GLfloat)viewport[2]) / viewport[3],
		0.1f,
		(float)globalSettings.getChunkRenderDistance() * Chunk::Width * 100
	);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));

	location = glGetUniformLocation(chunkShader, "fogColor");
	glUniform4fv(location, 1, glm::value_ptr(globalSettings.skyColor));
	location = glGetUniformLocation(chunkShader, "chunkRenderDistance");
	glUniform1i(location, globalSettings.getChunkRenderDistance());

	location = glGetUniformLocation(chunkShader, "model");
	for (const auto& p : chunkMap) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), (glm::vec3)toChunkMin(p.second.chunkPos));
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(p.second.VAO);
		glDrawElements(GL_TRIANGLES, (int)p.second.vertCount, GL_UNSIGNED_INT, 0);
	}
	glDisable(GL_CULL_FACE);

	drawPlayerLookHighlight(voxelShader, view, projection);
	drawPlayerUI(voxelShader);
}

void World::updateChunks() {
	const int loadDist = globalSettings.getChunkRenderDistance();
	const float timeLimit = 1.0f / 120.0f;
	glm::ivec2 cameraChunkPos = toChunkPos(player.camera.pos);
	std::vector<std::unordered_map<glm::ivec2, Chunk>::iterator> toErase = {};
	for (auto it = chunkMap.begin(); it != chunkMap.end(); it++) {
		glm::ivec2 dist = glm::abs(it->first - cameraChunkPos);
		if (dist.x > loadDist || dist.y > loadDist) {
			toErase.push_back(it);
		}
	}
	for (const auto& it : toErase) {
		if (!tryStoreChunk(it->second)) {
			std::cerr << "\nFailded to STORE chunk at ("
				<< it->first.x << ", "
				<< it->first.y << ")\n";
			continue;
		}
		glDeleteVertexArrays(1, &it->second.VAO);
		chunkMap.erase(it);
	}

	Timer timer = {};
	glm::ivec2 chunkOffset = {};
	for (chunkOffset.x = -loadDist; chunkOffset.x <= loadDist; chunkOffset.x++) {
		for (chunkOffset.y = -loadDist; chunkOffset.y <= loadDist; chunkOffset.y++) {
			//if (glm::abs(chunkOffset.x) + glm::abs(chunkOffset.y) > loadDist) continue;
			glm::ivec2 chunkPos = cameraChunkPos + chunkOffset;
			if (chunkMap.count(chunkPos)) continue;
			Chunk chunk;
			if (!tryLoadChunk(chunkPos, chunk)) {
				chunk = generateChunk(chunkPos);
			}
			
			rebuildChunkMesh(chunk);
			chunkMap.insert({ chunkPos, chunk });
			rebuildAdjacentChunkMeshes(chunkPos);
			if (timer.time() > timeLimit) return;
		}
	}
}

bool World::tryStoreChunk(const Chunk& chunk) {
	std::vector<unsigned char> bytes = compressChunk(chunk);
	std::string name = getChunkFilename(chunk.chunkPos);
	std::ofstream f(
		rootDirName + worldDirName + worldName + '\\' + chunkDirName + name, 
		std::ios::out | std::ios::binary | std::ios::trunc);
	if (f.fail()) return false;
	std::copy(
		bytes.begin(),
		bytes.end(),
		std::ostreambuf_iterator(f)
	);
	return true;
}

bool World::tryLoadChunk(const glm::ivec2& chunkPos, Chunk& chunk) {
	std::ifstream f(
		rootDirName + worldDirName + worldName + '\\' + chunkDirName + getChunkFilename(chunkPos),
		std::ios::in | std::ios::binary);
	if (f.fail()) return false;

	std::vector<unsigned char> bytes = {};
	std::copy(
		std::istreambuf_iterator<char>(f),
		std::istreambuf_iterator<char>(),
		std::back_inserter(bytes)
	);

	if (!tryDecompressChunk(bytes, chunk)) {
		std::cerr << "\nFailed to decompress chunk at ("
			<< chunkPos.x << ", "
			<< chunkPos.y << ") ";
		return false;
	}
	chunk.chunkPos = chunkPos;
	return true;
}

void World::drawPlayerUI(GLuint prog) const {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);  // is this good?
	GLfloat aspect = ((GLfloat)viewport[2]) / viewport[3];

	Mesh2 bar = Mesh2::makeQuad(
		{ glm::vec3(-0.015f, -0.015f, -1.0f), textureManager.getUVs({12, 8}) },
		{ glm::vec3(0.015f, 0.015f, 0.0f), textureManager.getBlockSize() }
	);
	GLuint barVAO = bar.buildIndexedVAO();
	glUseProgram(prog);
	glEnable(GL_BLEND);

	GLuint location = glGetUniformLocation(prog, "model");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

	location = glGetUniformLocation(prog, "view");
	glm::mat4 view = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, aspect, 1.0f));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));

	location = glGetUniformLocation(prog, "projection");

	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));

	location = glGetUniformLocation(prog, "colorTint");
	glUniform4fv(location, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));

	glBindVertexArray(barVAO);
	glDrawElements(GL_TRIANGLES, (int)bar.indices.size(), GL_UNSIGNED_INT, 0);
	glDisable(GL_BLEND);
	glDeleteVertexArrays(1, &barVAO);  // TODO: this may be wasteful.
}

void World::drawPlayerLookHighlight(
	GLuint prog, // there may be a better way to get shader...
	const glm::mat4& view,
	const glm::mat4& projection
) const {
	Raycast look = rayCast(player.camera.pos, player.camera.getDir(), globalSettings.playerLookDistance);
	if (!look.hasCollided) return;

	Mesh3 blockHighlight = Mesh3::makeVoxel(
		{ glm::vec3(-0.01f), textureManager.getUVs({5, 15}) },
		{ glm::vec3(1.02f), textureManager.getBlockSize() }
	);
	GLuint blockHighlightVAO = blockHighlight.buildIndexedVAO();

	glUseProgram(prog);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint location = glGetUniformLocation(prog, "model");
	glm::mat4 model = glm::translate(glm::mat4(1.0f), (glm::vec3)look.worldPos);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));

	location = glGetUniformLocation(prog, "view");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));

	location = glGetUniformLocation(prog, "projection");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));

	location = glGetUniformLocation(prog, "colorTint");
	glUniform4fv(location, 1, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 0.75f)));

	glBindVertexArray(blockHighlightVAO);
	glDrawElements(GL_TRIANGLES, (int)blockHighlight.indices.size(), GL_UNSIGNED_INT, 0);
	glDisable(GL_BLEND);

	glDeleteVertexArrays(1, &blockHighlightVAO);  // TODO: this is wasteful.
}

void World::movePlayerAsFreeCam() {
	float speed = globalSettings.playerFreeCamSpeed;
	glm::vec3 dir = player.camera.getDir();
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 forawrd = glm::normalize(glm::vec3(dir.x, 0.0f, dir.z));
	glm::vec3 right = glm::normalize(glm::cross(dir, up));

	float dt = (float)mainWindow->getDeltaTime();
	if (mainWindow->getKey(GLFW_KEY_W)) player.camera.pos += forawrd * speed * dt;
	if (mainWindow->getKey(GLFW_KEY_S)) player.camera.pos -= forawrd * speed * dt;
	if (mainWindow->getKey(GLFW_KEY_D)) player.camera.pos += right * speed * dt;
	if (mainWindow->getKey(GLFW_KEY_A)) player.camera.pos -= right * speed * dt;
	if (mainWindow->getKey(GLFW_KEY_SPACE)) player.camera.pos += up * speed * dt;
	if (mainWindow->getKey(GLFW_KEY_LEFT_SHIFT)) player.camera.pos -= up * speed * dt;
}

void World::movePlayerAsCollider() {
	float moveSpeed = globalSettings.playerColliderSpeed;
	static float ySpeed = 0.0f;  // This is bad.
	static float jumpTime = 0.0f;  // This is bad.

	const glm::vec3 groundCheckPos = glm::vec3(0.0, -0.2 -playerBodyHeight, 0.0);
	const glm::vec3 groundCheckSize = glm::vec3(0.8, 0.6, 0.8);

	const glm::vec3 playerMin = { 
		-playerWidth, 
		-playerBodyHeight, 
		-playerWidth 
	};
	const glm::vec3 playerMax = { 
		playerWidth, 
		playerFullHeight - playerBodyHeight, 
		playerWidth 
	};

	glm::vec3 delta = glm::vec3(0.0f);

	glm::vec3 dir = player.camera.getDir();
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 forawrd = glm::normalize(glm::vec3(dir.x, 0.0f, dir.z));
	glm::vec3 right = glm::normalize(glm::cross(dir, up));

	if (mainWindow->getKey(GLFW_KEY_W)) delta += forawrd;
	if (mainWindow->getKey(GLFW_KEY_S)) delta -= forawrd;
	if (mainWindow->getKey(GLFW_KEY_D)) delta += right;
	if (mainWindow->getKey(GLFW_KEY_A)) delta -= right;
	if (delta != glm::vec3(0.0f))delta = glm::normalize(delta);
	delta *= moveSpeed * mainWindow->getDeltaTime();

	bool isConstrained = boxCast(
		glm::floor(player.camera.pos + delta + playerMin),
		glm::floor(player.camera.pos + delta + playerMax)
	);
	if (isConstrained) {
		glm::vec3 axisDeltaX = glm::vec3(delta.x, 0.0f, 0.0f);
		bool isConstrainedX = boxCast(
			glm::floor(player.camera.pos + axisDeltaX + playerMin),
			glm::floor(player.camera.pos + axisDeltaX + playerMax)
		);
		if (isConstrainedX) delta.x = 0.0f;

		glm::vec3 axisDeltaZ = glm::vec3(0.0f, 0.0f, delta.z);
		bool isConstrainedZ = boxCast(
			glm::floor(player.camera.pos + axisDeltaZ + playerMin),
			glm::floor(player.camera.pos + axisDeltaZ + playerMax)
		);
		if (isConstrainedZ) delta.z = 0.0f;

		bool isStillConstrained = !isConstrainedX && !isConstrainedZ;
		if (isStillConstrained) {
			delta.x = 0.0f;
			delta.z = 0.0f;
		}
	}

	ySpeed -= playerGA * (float)mainWindow->getDeltaTime();
	delta.y += ySpeed * (float)mainWindow->getDeltaTime();

	bool isMovingDown = ySpeed < 0.0f;
	bool isConstrainedY = boxCast(
		glm::floor(player.camera.pos + delta + playerMin),
		glm::floor(player.camera.pos + delta + playerMax)
	);
	if (isConstrainedY) {
		ySpeed = 0;
		delta.y = 0;
	}

	bool isGrounded = (isMovingDown && isConstrainedY) || boxCast(
		glm::floor(player.camera.pos + groundCheckPos - groundCheckSize / 2.0f),
		glm::floor(player.camera.pos + groundCheckPos + groundCheckSize / 2.0f)
	);
	if (isGrounded && jumpTime >= 0.0f && mainWindow->getKeyDown(GLFW_KEY_SPACE)) {
		ySpeed = globalSettings.playerJumpSpeed;
		jumpTime = -playerJumpInterval;
	}

	player.camera.pos += delta;
	jumpTime += (float)mainWindow->getDeltaTime();
}

void World::updatePlayerInput() {
	const float mouseSpeed = globalSettings.mouseSpeed;
	const bool usePhysics = true;
	player.camera.setYaw(player.camera.getYaw() + (float)mainWindow->getMouseDeltaX() * mouseSpeed);
	player.camera.setPitch(player.camera.getPitch() - (float)mainWindow->getMouseDeltaY() * mouseSpeed);

	Raycast look = rayCast(
		player.camera.pos,
		player.camera.getDir(),
		globalSettings.playerLookDistance
	);
	if (look.hasCollided) {
		glm::ivec2 lookChunkPos = toChunkPos(look.worldPos);
		auto lookChunkIt = chunkMap.find(lookChunkPos);
		if (mainWindow->getKeyDown(GLFW_MOUSE_BUTTON_LEFT) && lookChunkIt != chunkMap.end()) {
			size_t blockIndex = toBlockIndex(toChunkLocal(look.worldPos));
			lookChunkIt->second.blocksCache[blockIndex] = BlockType::Air;
			rebuildChunkMesh(lookChunkIt->second);
			rebuildAdjacentChunkMeshes(lookChunkPos);
		}
		glm::ivec2 normalChunkPos = toChunkPos(look.worldPos + look.normal);
		auto normalChunkIt = chunkMap.find(normalChunkPos);
		if (mainWindow->getKeyDown(GLFW_MOUSE_BUTTON_RIGHT) && normalChunkIt != chunkMap.end()) {
			size_t blockIndex = toBlockIndex(toChunkLocal(look.worldPos + look.normal));
			normalChunkIt->second.blocksCache[blockIndex] = BlockType::Dirt;
			rebuildChunkMesh(normalChunkIt->second);
			rebuildAdjacentChunkMeshes(normalChunkPos);
		}
	}

	if (usePhysics) movePlayerAsCollider();
	else movePlayerAsFreeCam();
}

Raycast World::rayCast(
	const glm::vec3& pos,
	const glm::vec3& dir,
	float maxDistance
) const {
	const glm::ivec3 dirSign = glm::ivec3(
		dir.x >= 0 ? 1 : -1,
		dir.y >= 0 ? 1 : -1,
		dir.z >= 0 ? 1 : -1
	);  //  glm::sign() can result in 0, so it won't do.
	const glm::ivec3 dirPositive = glm::ivec3(
		dir.x >= 0 ? 1 : 0,
		dir.y >= 0 ? 1 : 0,
		dir.z >= 0 ? 1 : 0
	);
	glm::ivec3 worldPos = toIntPos(pos);

	while (true) {
		float dist;
		float minDist;
		glm::ivec3 delta = glm::ivec3(dirSign.x, 0, 0);
		glm::vec3 planeOrig = worldPos + dirPositive;
		glm::intersectRayPlane(pos, dir, planeOrig, glm::vec3(-dirSign.x, 0.0, 0.0), minDist);
		glm::intersectRayPlane(pos, dir, planeOrig, glm::vec3(0.0, -dirSign.y, 0.0), dist);
		if (dist < minDist) {
			minDist = dist;
			delta = glm::ivec3(0, dirSign.y, 0);
		}
		glm::intersectRayPlane(pos, dir, planeOrig, glm::vec3(0.0, 0.0, -dirSign.z), dist);
		if (dist < minDist) {
			minDist = dist;
			delta = glm::ivec3(0, 0, dirSign.z);
		}

		worldPos += delta;
		if (minDist > maxDistance
			|| worldPos.y < 0
			|| worldPos.y >= Chunk::Height
			) {
			return { false };
		}
		int block = getBlock(worldPos);
		if (block == blockNotReady) return { false };

		if (blockManager.blocks[block].phy.isSolid) {
			Raycast raycast = {};
			raycast.hasCollided = true;
			raycast.magnitude = minDist;
			raycast.normal = -delta;
			raycast.pos = pos + dir * minDist;
			raycast.worldPos = worldPos;
			return raycast;
		}
	}

	return { false };
}
