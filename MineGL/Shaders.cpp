#include "Shaders.hpp"

GLuint tryCompileShader(
	const GLchar* source,
	GLenum shaderType,
	GLsizei maxErrorLogSize,
	GLchar* errorLog
) {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, maxErrorLogSize, NULL, errorLog);
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

bool tryLinkShaderProgram(
	GLuint program,
	GLsizei maxErrorLogSize,
	GLchar* errorLog
) {
	glLinkProgram(program);
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, maxErrorLogSize, NULL, errorLog);
		return false;
	}
	return true;
}
