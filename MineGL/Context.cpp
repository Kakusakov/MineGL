#include "Context.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <stdexcept>

bool setupWindow(GLFWwindow* wnd, int w, int h) {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;
    glViewport(0, 0, w, h);
    glfwSetFramebufferSizeCallback(
        wnd,
        [](GLFWwindow* window, int width, int height) {
            glViewport(0, 0, width, height);
        }
    );
    return true;
}

Context::~Context() {
    if (glfwGetCurrentContext() == targetWindow) glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(targetWindow);
}

Context::Context() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, DEFAULT_GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, DEFAULT_GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    targetWindow = glfwCreateWindow(
        DEFAULT_WINDOW_WIDTH,
        DEFAULT_WINDOW_HEIGHT,
        DEFAULT_WINDOW_NAME,
        NULL,
        NULL
    );
    if (targetWindow == nullptr) throw std::logic_error("glfwCreateWindow failed");

    GLFWwindow* pervContext = glfwGetCurrentContext();
    glfwMakeContextCurrent(targetWindow);
    bool setupSuccess = setupWindow(
        targetWindow,
        DEFAULT_WINDOW_WIDTH,
        DEFAULT_WINDOW_HEIGHT
    );
    glfwMakeContextCurrent(pervContext);

    if (!setupSuccess) {
        glfwDestroyWindow(targetWindow);
        throw std::logic_error("setupWindow failed");
    }
}
