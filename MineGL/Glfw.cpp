#include "Glfw.hpp"

#include <stdexcept>
#include <iostream>

const std::string rootDirName = ".minegl\\";
const std::string resourceDirName = "Resouces\\";

// @throws std::runtime_error
Glfw::Glfw() {
	if (!glfwInit()) throw std::runtime_error("Unable to initialize glfw");
}
Glfw::~Glfw() {
	glfwMakeContextCurrent(nullptr);
	glfwTerminate();
}
// @throws std::runtime_error
void Glfw::initialize() {
	static Glfw glfw;
}

/*#include <cstdlib>

static bool isEngineReady = false;

bool isEngineActive() {
	return isEngineReady;
}

void terminateEngine() {
	glfwMakeContextCurrent(NULL);
	glfwTerminate();
}

bool trySetupEngine() { 
	if (isEngineReady) return true;
	if (!glfwInit()) return false;
	if (std::atexit(terminateEngine) != 0) {
		glfwTerminate();
		return false;
	}
	glfwSetErrorCallback(errorCallback);
	isEngineReady = true;
	return true;
}

static GLFWwindow* targetWindow = NULL;

GLFWwindow* getTargetWindow() {
	return targetWindow;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

bool tryMakeWindow(int width, int height) {
	if (targetWindow != NULL) return false;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	targetWindow = glfwCreateWindow(width, height, "MineGL", NULL, NULL);
	if (!targetWindow) return false;
	glfwMakeContextCurrent(targetWindow);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwMakeContextCurrent(NULL);
		glfwDestroyWindow(targetWindow);
		targetWindow = NULL;
		return false;
	}
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(targetWindow, framebufferSizeCallback);
	return true;
}*/
