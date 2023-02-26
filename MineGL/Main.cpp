#include "EngineCore.hpp"
#include "EngineFileIO.hpp"
#include "Shaders.hpp"
#include "Textures.hpp"
#include "EngineInput.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>


const size_t bufferSize = 1024;
char buffer[bufferSize];

bool tryLoadResources(
	GLuint& vertexShader,
	GLuint& fragmentShader,
	GLuint& atlas
) {
	std::string temp;
	if (!tryLoadTextFile("Resources\\Shaders\\chunk_vertex.txt", temp)) {
		std::cout << "Failed to load vertex shader, error: " << buffer << '\n';
		return false;
	}
	vertexShader = tryCompileShader(temp.c_str(), GL_VERTEX_SHADER, bufferSize, buffer);
	if (!tryLoadTextFile("Resources\\Shaders\\chunk_fragment.txt", temp)) {
		std::cout << "Failed to load fragment shader, error: " << buffer << '\n';
		return false;
	}
	fragmentShader = tryCompileShader(temp.c_str(), GL_FRAGMENT_SHADER, bufferSize, buffer);
	atlas = tryLoadTexture("Resources\\atlas.jpg", GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	return vertexShader && fragmentShader && atlas;
}

bool tryCreateShaderProgram(
	GLuint& shaderProg,
	GLuint vertexShader, 
	GLuint fragmentShader
) {
	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertexShader);
	glAttachShader(shaderProg, fragmentShader);
	if (!tryLinkShaderProgram(shaderProg, bufferSize, buffer)) {
		std::cout << "Failed to link shader program, error: " << buffer << '\n';
		return false;
	}
	return true;
}

bool trySetupResources(GLuint& shaderProg, GLuint& atlas) {
	GLuint vsh, fsh;
	if (!tryLoadResources(vsh, fsh, atlas)) {
		std::cout << "Failed to load resources.\n";
		return false;
	}
	if (!tryCreateShaderProgram(shaderProg, vsh, fsh)) {
		std::cout << "Failed to create shader program.\n";
		return false;
	}
	glDeleteShader(vsh);
	glDeleteShader(fsh);
	return true;
}

void generateBufferObjects(GLuint& VAO, int atalsX, int atalsY) {
	GLfloat maxX = 1.0f / 16 * (atalsX + 1);
	GLfloat maxY = 1.0f / 16 * ((15 - atalsY) + 1);
	GLfloat minX = 1.0f / 16 * atalsX;
	GLfloat minY = 1.0f / 16 * (15 - atalsY);

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
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void generateMatrices(
	glm::mat4& model, 
	glm::mat4& view, 
	glm::mat4& projection,
	const glm::vec3& cameraPos,
	const glm::vec3& cameraDir
) {
	model = glm::mat4(1.0f);
	//model = glm::rotate(model, modelAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::lookAt(cameraPos, cameraPos + cameraDir, glm::vec3(0.0f, 1.0f, 0.0f));
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

void setUniformMatrices(
	GLuint shaderProgram,
	const glm::mat4& model,
	const glm::mat4& view,
	const glm::mat4& projection
) {
	GLint location = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
	location = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));
	location = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));
}

void registerInputKeys() {
	registerKey(GLFW_KEY_W);
	registerKey(GLFW_KEY_A);
	registerKey(GLFW_KEY_S);
	registerKey(GLFW_KEY_D);

	registerKey(GLFW_KEY_SPACE);
	registerKey(GLFW_KEY_LEFT_SHIFT);
}

int main() {
	if (!trySetupEngine()) {
		std::cout << "Engine setup failed.\n";
		return -1;
	}
	if (!tryMakeWindow(800, 600)) {
		std::cout << "Failed to create a window.\n";
		return -1;
	}
	GLuint prog, atlas;
	if (!trySetupResources(prog, atlas)) {
		std::cout << "Failed to setup resources.\n";
		return -1;
	}
	registerInputKeys();
	GLuint VAO;
	generateBufferObjects(VAO, 3, 0);
	glm::mat4 model, view, projection;
	auto cameraPos = glm::vec3(0.0f);
	auto cameraDir = glm::vec3(0.0f, 0.0f, 1.0f);
	const float cameraSpeed = 5.0f;
	const float mouseSpeed = 0.05f;
	float yaw = -90.0f;
	float pitch = 0.0f;

	glfwSetInputMode(getTargetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glUseProgram(prog);
	glBindTexture(GL_TEXTURE_2D, atlas);
	glBindVertexArray(VAO);
	glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
	GLFWwindow* target = getTargetWindow();
	while (!glfwWindowShouldClose(target)) {
		glClear(GL_COLOR_BUFFER_BIT);
		cameraDir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDir.y = sin(glm::radians(pitch));
		cameraDir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		generateMatrices(model, view, projection, cameraPos, cameraDir);
		setUniformMatrices(prog, model, view, projection);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(target);
		updateInput();
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraForawrd = glm::normalize(glm::vec3(cameraDir.x, 0.0f, cameraDir.z));
		glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDir, cameraUp));
		if (getKey(GLFW_KEY_W)) cameraPos += cameraForawrd * cameraSpeed * getDeltaTime();
		if (getKey(GLFW_KEY_S)) cameraPos -= cameraForawrd * cameraSpeed * getDeltaTime();
		if (getKey(GLFW_KEY_D)) cameraPos += cameraRight * cameraSpeed * getDeltaTime();
		if (getKey(GLFW_KEY_A)) cameraPos -= cameraRight * cameraSpeed * getDeltaTime();
		if (getKey(GLFW_KEY_SPACE)) cameraPos += cameraUp * cameraSpeed * getDeltaTime();
		if (getKey(GLFW_KEY_LEFT_SHIFT)) cameraPos -= cameraUp * cameraSpeed * getDeltaTime();
		yaw = glm::mod(yaw + getMouseDeltaX() * mouseSpeed, 360.0f);
		pitch = glm::clamp(pitch - getMouseDeltaY() * mouseSpeed, -89.0f, 89.0f);
	}

	return 0;
}
