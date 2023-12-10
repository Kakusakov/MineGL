#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <string>

// TODO: this should be moved to another header.
extern const std::string rootDirName;
extern const std::string resourceDirName;

class Glfw {
	Glfw();
	~Glfw();
public:
	// @throws std::runtime_error
	static void initialize();
};
