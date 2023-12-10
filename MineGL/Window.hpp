#pragma once
#include "Glfw.hpp"
#include <set>
#include <unordered_map>
#include <memory>

class Window {
private:
	GLFWwindow* window = nullptr;

	std::set<int> keysReg = {};
	std::set<int> mouseButtonReg = {};

	// Key and mouse button enums do not overlap, so they can share state maps.
	std::unordered_map<int, bool> keysPressedNow = {};
	std::unordered_map<int, bool> keysPressedLast = {};

	double time = 0.0;
	double deltaTime = 0.0;

	double cursorPosX = 0.0;
	double cursorPosY = 0.0;
	double cursorPosXDelta = 0.0;
	double cursorPosYDelta = 0.0;

	void updateKeyInput();
	void updateTime();
	void updateMouseInput();
public:
	static constexpr double maxDeltaTime = 0.5;

	Window(const Window&) = delete;
	void operator=(const Window&) = delete;

	// @throws std::runtime_error
	Window(int width, int height);
	~Window();

	void makeContextCurrent() const;

	bool shouldClose() const;

	int getInputMode(int mode) const;
	void setInputMode(int mode, int value);

	bool isKeyRegistered(int key) const;
	void registerKey(int key);

	bool isMouseButtonRegistered(int button) const;
	void registerMouseButton(int button);

	bool getKeyDown(int key) const;
	bool getKeyUp(int key) const;
	bool getKeyHeld(int key) const;

	bool getKey(int key) const;

	double getTime() const;
	double getDeltaTime() const;

	double getMousePosX() const;
	double getMousePosY() const;
	double getMouseDeltaX() const;
	double getMouseDeltaY() const;

	void update();
};

extern std::unique_ptr<Window> mainWindow;
