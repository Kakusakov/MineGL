#pragma once
#include <memory>

#if !defined(DEFAULT_WINDOW_WIDTH)
#define DEFAULT_WINDOW_WIDTH 800
#endif
#if !defined(DEFAULT_WINDOW_HEIGHT)
#define DEFAULT_WINDOW_HEIGHT 600
#endif
#if !defined(DEFAULT_WINDOW_NAME)
#define DEFAULT_WINDOW_NAME "MineGL Engine"
#endif

#if !defined(DEFAULT_GL_VERSION_MAJOR)
#define DEFAULT_GL_VERSION_MAJOR 3
#endif
#if !defined(DEFAULT_GL_VERSION_MINOR)
#define DEFAULT_GL_VERSION_MINOR 3
#endif

struct GLFWwindow;

class Context {
private:
	GLFWwindow* targetWindow = nullptr;
public:
	Context(const Context&) = delete;
	void operator=(const Context&) = delete;
	Context();
	~Context();
	void SwapBuffers();
	void ShouldClose();
};




