#include "EngineInput.hpp"
#include "EngineCore.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>

static std::vector<int> keysReg = {};  // TODO: maybe use std::set instead?
static std::unordered_map<int, bool> keysPressedNow = {};
static std::unordered_map<int, bool> keysPressedLast = {};

bool isKeyRegistered(int key) {
	return std::binary_search(keysReg.begin(), keysReg.end(), key);
}

void registerKey(int key) {
	if (isKeyRegistered(key)) return;
	keysReg.insert(std::upper_bound(keysReg.begin(), keysReg.end(), key), key);
	bool isPressed = glfwGetKey(getTargetWindow(), key);
	keysPressedLast.insert({ key, isPressed });
	keysPressedNow.insert({ key, isPressed });
}

bool getKeyDown(int key) {
	auto itNow = keysPressedNow.find(key);
	auto itLast = keysPressedLast.find(key);
	return itNow != keysPressedNow.end() 
		&& itNow->second
		&& !itLast->second;
}

bool getKeyUp(int key) {
	auto itNow = keysPressedNow.find(key);
	auto itLast = keysPressedLast.find(key);
	return itNow != keysPressedNow.end()
		&& !itNow->second
		&& itLast->second;
}

bool getKeyHeld(int key) {
	auto itNow = keysPressedNow.find(key);
	auto itLast = keysPressedLast.find(key);
	return itNow != keysPressedNow.end()
		&& itNow->second
		&& itLast->second;
}

bool getKey(int key) {
	auto it = keysPressedNow.find(key);
	return it != keysPressedNow.end() && it->second;
}

void updateKeyInput() {
	GLFWwindow *target = getTargetWindow();
	for (const int& key : keysReg) {
		auto it = keysPressedNow.find(key);
		keysPressedLast[key] = it->second;
		it->second = glfwGetKey(target, key);
	}
}

static double timeLast;
static double timeNow;

float getTime() {
	return timeNow;
}

float getDeltaTime() {
	return timeNow - timeLast;
}

void updateTime() {
	timeLast = timeNow;
	timeNow = glfwGetTime();
}

static double cursorPosXNow;
static double cursorPosYNow;
static double cursorPosXLast;
static double cursorPosYLast;

float getMousePosX() {
	return cursorPosXNow;
}

float getMousePosY() {
	return cursorPosYNow;
}

float getMouseDeltaX() {
	return cursorPosXNow - cursorPosXLast;
}

float getMouseDeltaY() {
	return cursorPosYNow - cursorPosYLast;
}

void updateMouseInput() {
	cursorPosXLast = cursorPosXNow;
	cursorPosYLast = cursorPosYNow;
	glfwGetCursorPos(getTargetWindow(), &cursorPosXNow, &cursorPosYNow);
}

void updateInput() {
	glfwPollEvents();
	static bool initialUpdate = true;
	if (initialUpdate) {
		updateTime();
		updateMouseInput();
		initialUpdate = false;
	}
	updateKeyInput();
	updateTime();
	updateMouseInput();
}
