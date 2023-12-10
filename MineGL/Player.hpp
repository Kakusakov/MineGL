#pragma once
#include "Glfw.hpp"
#include <glm/glm.hpp>

// TODO: assign as a member of player 
// TODO: this is not a good representation of colliders
const float playerWidth = 0.4f;
const float playerFullHeight = 1.7f;
const float playerBodyHeight = 1.5f;
const float playerJumpInterval = 0.25f;

// TODO: this has nothing to do with player
const float playerGA = 9.8f;

struct Camera {
private:
	float yaw = -90.0f;
	float pitch = 0.0f;
public:
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);

	inline float getYaw() const { return yaw; }
	inline float getPitch() const { return pitch; }
	inline void setYaw(float yaw) {
		this->yaw = glm::mod(yaw, 360.0f);
	}
	inline void setPitch(float pitch) {
		this->pitch = glm::clamp(pitch, -89.0f, 89.0f);
	}
	inline const glm::vec3 getDir() const { 
		return {
			cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
			sin(glm::radians(pitch)),
			sin(glm::radians(yaw)) * cos(glm::radians(pitch))
		};
	}
};

struct Player {
public:
	Camera camera;
};
