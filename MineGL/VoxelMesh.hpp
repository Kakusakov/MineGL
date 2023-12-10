#pragma once
#include "Glfw.hpp"
#include "Directions.hpp"

#include <vector>

struct Mesh3 {
public:
	struct Vertex {
	public:
		glm::vec3 pos;
		glm::vec2 texCoord;
	};

	std::vector<Vertex> vertices = {};
	std::vector<GLint> indices = {};

	static Mesh3 makeVoxel(Vertex min, Vertex max);

	GLuint buildIndexedVAO() const;
};

struct Mesh2 {
public:
	struct Vertex {
	public:
		glm::vec2 pos;
		glm::vec2 texCoord;
	};

	std::vector<Vertex> vertices = {};
	std::vector<GLint> indices = {};

	static Mesh2 makeQuad(Vertex min, Vertex max);

	GLuint buildIndexedVAO() const;
};
