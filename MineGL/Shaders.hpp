#pragma once
#include "EngineCore.hpp"

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
