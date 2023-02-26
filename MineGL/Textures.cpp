#include "Textures.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb_image.h"

GLuint tryLoadTexture(
	const char* path, 
	GLint internalFormat, 
	GLenum format, 
	GLenum type
) {
	int width, height, channelsCount;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &channelsCount, 0);
	if (!data) return 0;

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		internalFormat,
		width, 
		height, 
		0, 
		format,
		type,
		data
	);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}
