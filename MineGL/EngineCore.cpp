#include "EngineCore.h"

#include <cstdlib>
#include <iostream>

static bool isEngineReady = false;

bool isEngineActive() {
	return isEngineReady;
}

void terminateEngine() {
	// TODO: guarantee all threads don't have any active OGL contexts
	glfwMakeContextCurrent(NULL);
	glfwTerminate();
}

void errorCallback(int errorCode, const char* description) {
	std::cerr << "\n[GLFW ERROR] code="
		<< errorCode
		<< " description: "
		<< description
		<< "\n";
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
}
