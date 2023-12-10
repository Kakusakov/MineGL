#pragma once
#include "Glfw.hpp"

GLuint tryCompileShader(
	const GLchar* source,
	GLenum shaderType,
	GLsizei maxErrorLogSize,
	GLchar* errorLog
);

bool tryLinkShaderProgram(
	GLuint program,
	GLsizei maxErrorLogSize,
	GLchar* errorLog
);
