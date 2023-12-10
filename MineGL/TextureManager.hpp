#pragma once
#include <glm/glm.hpp>
#include <array>

// TODO: this is clost to unnecessary wrapping
class TextureManager {
public:
	GLuint atlasTexture = 0;
	glm::vec<2, size_t> atlasSize = {};
	inline glm::vec2 getBlockSize() const {
		return {
			1.0f / atlasSize.x,
			1.0f / atlasSize.y
		};
	}
	inline glm::vec2 getUVs(glm::vec<2, size_t> atlasPos) const {
		glm::vec2 uvs = getBlockSize();
		uvs.x *= atlasPos.x;
		uvs.y *= (atlasSize.y - 1 - atlasPos.y);
		return uvs;
	}
};
