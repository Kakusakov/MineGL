#include "Window.hpp"

#include <stdexcept>
#include <algorithm>

std::unique_ptr<Window> mainWindow;

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

Window::Window(int width, int height) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(
		width, height, "MineGL", nullptr, nullptr);
	if (!window) throw std::runtime_error("Unable to create window");

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwMakeContextCurrent(nullptr);
		glfwDestroyWindow(window);
		throw std::runtime_error(
			"Unable to load opengl function pointers to window context");
	}

	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwPollEvents();
	time = glfwGetTime();
	glfwGetCursorPos(window, &cursorPosX, &cursorPosY);
}

Window::~Window() {
	if (!window) return;
	glfwMakeContextCurrent(nullptr);
	glfwDestroyWindow(window);
}

void Window::makeContextCurrent() const {
	glfwMakeContextCurrent(window);
}

int Window::getInputMode(int mode) const {
	return glfwGetInputMode(window, mode);
}

void Window::setInputMode(int mode, int value) {
	glfwSetInputMode(window, mode, value);
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(window);
}

bool Window::isMouseButtonRegistered(int button) const {
	return mouseButtonReg.count(button) != 0;
}

void Window::registerMouseButton(int button) {
	if (isKeyRegistered(button)) return;
	mouseButtonReg.insert(button);
	bool isPressed = glfwGetMouseButton(window, button);
	keysPressedLast.insert({ button, isPressed });
	keysPressedNow.insert({ button, isPressed });
}

bool Window::isKeyRegistered(int key) const {
	return keysReg.count(key) != 0;
}

void Window::registerKey(int key) {
	if (isKeyRegistered(key)) return;
	keysReg.insert(key);
	bool isPressed = glfwGetKey(window, key);
	keysPressedLast.insert({ key, isPressed });
	keysPressedNow.insert({ key, isPressed });
}

bool Window::getKeyDown(int key) const {
	auto itNow = keysPressedNow.find(key);
	auto itLast = keysPressedLast.find(key);
	return itNow != keysPressedNow.end()
		&& itNow->second
		&& !itLast->second;
}

bool Window::getKeyUp(int key) const {
	auto itNow = keysPressedNow.find(key);
	auto itLast = keysPressedLast.find(key);
	return itNow != keysPressedNow.end()
		&& !itNow->second
		&& itLast->second;
}

bool Window::getKeyHeld(int key) const {
	auto itNow = keysPressedNow.find(key);
	auto itLast = keysPressedLast.find(key);
	return itNow != keysPressedNow.end()
		&& itNow->second
		&& itLast->second;
}

bool Window::getKey(int key) const {
	auto it = keysPressedNow.find(key);
	return it != keysPressedNow.end() && it->second;
}

void Window::updateKeyInput() {
	for (const int& key : keysReg) {
		auto it = keysPressedNow.find(key);
		keysPressedLast[key] = it->second;
		it->second = glfwGetKey(window, key);
	}
	for (const int& button : mouseButtonReg) {
		auto it = keysPressedNow.find(button);
		keysPressedLast[button] = it->second;
		it->second = glfwGetMouseButton(window, button);
	}
}

double Window::getTime() const {
	return time;
}

double Window::getDeltaTime() const {
	return deltaTime;
}

void Window::updateTime() {
	double now = glfwGetTime();
	deltaTime = std::min(now - time, maxDeltaTime);
	time = now;
}

double Window::getMousePosX() const {
	return cursorPosX;
}

double Window::getMousePosY() const {
	return cursorPosY;
}

double Window::getMouseDeltaX() const {
	return cursorPosXDelta;
}

double Window::getMouseDeltaY() const {
	return cursorPosYDelta;
}

void Window::updateMouseInput() {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	cursorPosXDelta = x - cursorPosX;
	cursorPosYDelta = y - cursorPosY;
	cursorPosX = x;
	cursorPosY = y;
}

void Window::update() {
	glfwSwapBuffers(window);
	glfwPollEvents();
	updateKeyInput();
	updateTime();
	updateMouseInput();
}
