#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

bool isEngineActive();
bool trySetupEngine();

GLFWwindow* getTargetWindow();
bool tryMakeWindow(int width, int height);

