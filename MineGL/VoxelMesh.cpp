#include "VoxelMesh.hpp"

/*
	GLfloat vertices[] = {
		// positions         // texture coords
		 0.5f,  0.5f, 0.0f,   maxX, maxY,   // top right
		 0.5f, -0.5f, 0.0f,   maxX, minY,   // bottom right
		-0.5f, -0.5f, 0.0f,   minX, minY,   // bottom left
		-0.5f,  0.5f, 0.0f,   minX, maxY    // top left
	};
	GLint indices[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};
*/

GLuint Mesh3::buildIndexedVAO() const {
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	return VAO;
}

Mesh3 Mesh3::makeVoxel(Vertex min, Vertex size) {
	Mesh3 mesh = {};
	for (int side = 0; side < 6; side++) {
		mesh.vertices.push_back({
			min.pos + size.pos * (glm::vec3)voxelEdgeOffsets[side][0],
			min.texCoord + size.texCoord * voxelTexCoords[0]
		});
		mesh.vertices.push_back({
			min.pos + size.pos * (glm::vec3)voxelEdgeOffsets[side][1],
			min.texCoord + size.texCoord * voxelTexCoords[1]
		});
		mesh.vertices.push_back({
			min.pos + size.pos * (glm::vec3)voxelEdgeOffsets[side][2],
			min.texCoord + size.texCoord * voxelTexCoords[2]
		});
		mesh.vertices.push_back({
			min.pos + size.pos * (glm::vec3)voxelEdgeOffsets[side][3],
			min.texCoord + size.texCoord * voxelTexCoords[3]
		});
		int lastInd = mesh.indices.empty() ? 0 : mesh.indices.back() + 1;
		mesh.indices.push_back(lastInd);
		mesh.indices.push_back(lastInd + 1);
		mesh.indices.push_back(lastInd + 3);
		mesh.indices.push_back(lastInd + 1);
		mesh.indices.push_back(lastInd + 2);
		mesh.indices.push_back(lastInd + 3);
	}
	return mesh;
}



GLuint Mesh2::buildIndexedVAO() const {
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	return VAO;
}

Mesh2 Mesh2::makeQuad(Vertex min, Vertex size) {
	Mesh2 mesh = {};
	mesh.vertices.push_back({
			min.pos + size.pos * (glm::vec2)voxelEdgeOffsets[BlockSide::BlockFront][0],
			min.texCoord + size.texCoord * voxelTexCoords[0]
	});
	mesh.vertices.push_back({
		min.pos + size.pos * (glm::vec2)voxelEdgeOffsets[BlockSide::BlockFront][1],
		min.texCoord + size.texCoord * voxelTexCoords[1]
	});
	mesh.vertices.push_back({
		min.pos + size.pos * (glm::vec2)voxelEdgeOffsets[BlockSide::BlockFront][2],
		min.texCoord + size.texCoord * voxelTexCoords[2]
	});
	mesh.vertices.push_back({
		min.pos + size.pos * (glm::vec2)voxelEdgeOffsets[BlockSide::BlockFront][3],
		min.texCoord + size.texCoord * voxelTexCoords[3]
	});
	mesh.indices = { 0, 1, 3, 1, 2, 3 };
	return mesh;
}

