#include "Window.hpp"
#include "Shaders.hpp"
#include "World.hpp"
#include "Settings.hpp"
#include "Texture.hpp"
#include "Window.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <sstream>

const size_t bufferSize = 1024;
char buffer[bufferSize];  // this is not thread-safe

bool tryLoadShaderFromFolder(
	GLuint& vertexShader,
	GLuint& fragmentShader,
	const std::string& folderPath
) {
	std::string temp;
	std::string traceback = "from folder \"" + folderPath + '\"';
	std::ifstream f(rootDirName + folderPath + "\\vertex.txt");
	if (f.fail()) {
		std::cout << "Failed to load vertex shader " + traceback + ".\n";
		return false;
	}

	std::stringstream ss;
	ss << f.rdbuf();
	temp = ss.str();
	vertexShader = tryCompileShader(temp.c_str(), GL_VERTEX_SHADER, bufferSize, buffer);
	if (!vertexShader) {
		std::cout << "Failed to compile vertex shader " + traceback + ", error: " 
			<< buffer << "\n";
		return false;
	}

	std::ifstream f2(rootDirName + folderPath + "\\fragment.txt");
	if (f.fail()) {
		std::cout << "Failed to load fragment shader " + traceback + "\n";
		return false;
	}

	std::stringstream ss2;
	ss2 << f2.rdbuf();
	temp = ss2.str();
	fragmentShader = tryCompileShader(temp.c_str(), GL_FRAGMENT_SHADER, bufferSize, buffer);
	if (!fragmentShader) {
		std::cout << "Failed to compile fragment shader " + traceback + ", error: "
			<< buffer << "\n";
		return false;
	}
	return true;
}

bool tryCreateShaderProgram(
	GLuint& shaderProg,
	GLuint vertexShader,
	GLuint fragmentShader
) {
	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertexShader);
	glAttachShader(shaderProg, fragmentShader);
	if (!tryLinkShaderProgram(shaderProg, bufferSize, buffer)) {
		std::cout << "Failed to link shader program, error: " << buffer << '\n';
		return false;
	}
	return true;
}

bool tryLoadResources(
	GLuint& chunkShaderProg,
	GLuint& voxelMeshShaderProg,
	GLuint& atlas
) {
	std::string temp;
	GLuint vsh, fsh;
	if (!tryLoadShaderFromFolder(vsh, fsh, "Resources\\Shaders\\chunk")) {
		std::cout << "Failed to load chunk shdaer prog.\n";
		return false;
	}
	if (!tryCreateShaderProgram(chunkShaderProg, vsh, fsh)) {
		std::cout << "Failed to create chunk shader prog.\n";
		return false;
	}
	if (!tryLoadShaderFromFolder(vsh, fsh, "Resources\\Shaders\\voxel_mesh")) {
		std::cout << "Failed to load voxel mesh shdaer prog.\n";
		return false;
	}
	if (!tryCreateShaderProgram(voxelMeshShaderProg, vsh, fsh)) {
		std::cout << "Failed to create voxel mesh shader prog.\n";
		return false;
	}
	atlas = tryLoadTexture("Resources\\Textures\\atlas_v1.png", GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	if (!atlas) {
		std::cout << "Failed to load texture atlas.\n";
		return false;
	}
	return true;
}

void setupMainWindow() {
	Glfw::initialize();

	mainWindow.reset(new Window(800, 600));

	mainWindow->registerKey(GLFW_KEY_W);
	mainWindow->registerKey(GLFW_KEY_A);
	mainWindow->registerKey(GLFW_KEY_S);
	mainWindow->registerKey(GLFW_KEY_D);

	mainWindow->registerKey(GLFW_KEY_SPACE);
	mainWindow->registerKey(GLFW_KEY_LEFT_SHIFT);

	mainWindow->registerMouseButton(GLFW_MOUSE_BUTTON_LEFT);
	mainWindow->registerMouseButton(GLFW_MOUSE_BUTTON_RIGHT);

	mainWindow->setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mainWindow->makeContextCurrent();
}

TextureManager generateTextureManager(glm::vec<2, size_t> atlasSize, GLuint atlas) {
	TextureManager textureManager;
	textureManager.atlasSize = atlasSize;
	textureManager.atlasTexture = atlas;
	return textureManager;
}

BlockManager generateBlockManager(const TextureManager& textureManager) {
	BlockManager bm = { std::vector<BlockData>(BlockType::BlocksTypesCount) };
	glm::vec2 er = textureManager.getUVs({ 12, 8 });
	glm::vec2 bd = textureManager.getUVs({ 1, 1 });
	glm::vec2 st = textureManager.getUVs({ 0, 3 });
	glm::vec2 dt = textureManager.getUVs({ 0, 2 });
	glm::vec2 gs = textureManager.getUVs({ 0, 1 });
	glm::vec2 gt = textureManager.getUVs({ 0, 0 });
	bm.blocks[BlockType::Air].tex = { true, { er, er, er, er, er, er } };
	bm.blocks[BlockType::Bedrock].tex = { false, { bd, bd, bd, bd, bd, bd } };
	bm.blocks[BlockType::Stone].tex = { false, { st, st, st, st, st, st } };
	bm.blocks[BlockType::Dirt].tex = { false, { dt, dt, dt, dt, dt, dt } };
	bm.blocks[BlockType::Grass].tex = { false, { gt, dt, gs, gs, gs, gs } };

	bm.blocks[BlockType::Air].phy = { false };
	bm.blocks[BlockType::Bedrock].phy = { true };
	bm.blocks[BlockType::Stone].phy = { true };
	bm.blocks[BlockType::Dirt].phy = { true };
	bm.blocks[BlockType::Grass].phy = { true };
	return bm;
}

/*#include <glm/gtc/noise.hpp>
#include "Perlin.hpp"
#include <chrono>

void benchmarkNoise(size_t seed, size_t testSize) {
	std::cout << "seed=" << seed << '\n';
	std::cout << "testSize=" << testSize << '\n';

	std::cout << "testing custom perlin generator" << std::endl;
	std::cout << "Seting up perlin generator..." << std::endl;
	std::chrono::steady_clock::time_point start, end;
	start = std::chrono::steady_clock::now();
	Perlin perlin(seed);
	end = std::chrono::steady_clock::now();
	std::cout << "generator setup took"
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(start - end).count()
		<< "ns" << std::endl;
	start = std::chrono::steady_clock::now();
	for (size_t i = 0; i < testSize; i++) {
		perlin.perlin3(i * 0.35236236f, i * 213.12312541351f, i * 93.12335473f);
	}
	end = std::chrono::steady_clock::now();
	std::cout << "test took"
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(start - end).count()
		<< "ns" << std::endl;

	std::cout << "testing glm perlin generator..." << std::endl;
	glm::vec3 v;
	start = std::chrono::steady_clock::now();
	for (size_t i = 0; i < testSize; i++) {
		v.x = i * 0.35236236f;
		v.y = i * 213.12312541351f;
		v.z = i * 93.12335473f;
		glm::perlin(v);
	}
	end = std::chrono::steady_clock::now();
	std::cout << "test took"
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(start - end).count()
		<< "ns" << std::endl;
}*/

void errorCallback(int errorCode, const char* description) {
	std::cerr << "\n[GLFW ERROR] code="
		<< errorCode
		<< " description: "
		<< description
		<< "\n";
}

int main() {
	glfwSetErrorCallback(errorCallback);
	try {
		setupMainWindow();
	}
	catch (std::runtime_error e) {
		std::cerr << "Faied to create window: " << e.what() << '\n';
		return -1;
	}

	GLuint chunkProg, voxelMeshProg, atlas;
	if (!tryLoadResources(chunkProg, voxelMeshProg, atlas)) {
		std::cout << "Failed to setup resources.\n";
		return -1;
	}
	glUseProgram(chunkProg);
	glBindTexture(GL_TEXTURE_2D, atlas);
	glClearColor(
		globalSettings.skyColor.x,
		globalSettings.skyColor.y,
		globalSettings.skyColor.z,
		globalSettings.skyColor.w
	);
	glEnable(GL_DEPTH_TEST);
	//TerrainGenerator tg = { { {(unsigned)time(nullptr)}, 1, 0.01, 60.0f, 0.0f, 0.0f }, 4 };
	//TerrainGenerator tg = { { {(unsigned)time(nullptr)}, 1, 0.1, 6.0f, 0.0f, 0.0f }, 4 };
	TerrainGenerator tg = { { {(unsigned)time(nullptr)}, 3, 0.01f, 60.0f, 10.0f, 0.1f }, 4 };
	TextureManager tm = generateTextureManager({ 16, 16 }, atlas);
	BlockManager bm = generateBlockManager(tm);
	std::string worldName = "Seeded" 
		+ std::to_string(tg.noiseGenerator.noiseGenerator.getSeed());
	std::unique_ptr<World> w;
	try {
		w = std::make_unique<World>(bm, tm, tg, worldName);
	}
	catch (std::exception e) {  // should probaly catch something more specific
		std::cerr << "Failed to create world: " << e.what() << '\n';
		return -1;
	}
	w->teleportPlayer({ 0, 90, 0 });
	std::vector<double> frameTimes = {};
	while (!mainWindow->shouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		w->update(chunkProg, voxelMeshProg);
		mainWindow->update();

		for (int i = (int)frameTimes.size() - 1; i >= 0; i--) {
			if (mainWindow->getTime() - frameTimes[i] > 1.0) {
				frameTimes.erase(frameTimes.cbegin() + i);
			}
		}
		frameTimes.push_back(mainWindow->getTime());

		glm::ivec3 wpBlock = w->rayCast(
			w->getPlayer().camera.pos,
			glm::normalize(w->getPlayer().camera.getDir()),
			(float)globalSettings.getChunkRenderDistance() * Chunk::Width * 2
		).worldPos;

		std::cout << std::string(64, ' ') << '\r';
		std::cout << "look=["
			<< wpBlock.x << ", " 
			<< wpBlock.y << ", " 
			<< wpBlock.z << "]"
			<< "   "
			<< "fps=" << frameTimes.size()
			<< '\r' << std::flush;
	}
	mainWindow.release();  // avoid outliving the glfw library

	return 0;
}
