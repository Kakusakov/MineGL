#pragma once
#include "Glfw.hpp"

#include <string>

GLuint tryLoadTexture(
	std::string path,
	GLint internalFormat,
	GLenum format,
	GLenum type
);
