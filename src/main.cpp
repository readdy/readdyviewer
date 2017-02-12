#include <glbinding/Binding.h>
#include "common.h"

using namespace gl;
namespace rv {
    GLFWwindow *window = NULL;

    void glfwErrorCallback(int error, const char *msg) {
        rv::log::error("GLFW error {}: {}", error, msg);
    }

    void glDebugCallback(gl::GLenum source, gl::GLenum type, gl::GLuint id, gl::GLenum severity, gl::GLsizei length,
                         const gl::GLchar *message,
                         const void *userParam) {
        log::debug("OpenGL debug message: {}", std::string(message, static_cast<std::size_t>(length)));
    }

    void initialize(bool debugContext) {
        glbinding::Binding::initialize();
        if (!glfwInit()) {
            throw std::runtime_error("Could not initialize GLFW!");
        }
        glfwSetErrorCallback(glfwErrorCallback);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugContext ? GL_TRUE : GL_FALSE);

        window = glfwCreateWindow(1280, 720, "ReaDDy viewer", NULL, NULL);
        if (window == NULL) {
            throw std::runtime_error("Cannot open window.");
        }
        glfwMakeContextCurrent(window);

        if (debugContext) {
            glDebugMessageCallback(glDebugCallback, NULL);
            glEnable(GL_DEBUG_OUTPUT);
        }

        while (!glfwWindowShouldClose(window)) {
            glfwSwapBuffers (window);
            glfwPollEvents ();
        }
    }

    void cleanup() {
        if (window != NULL) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }


}

int main() {
    try {
        rv::initialize(true);
        rv::cleanup();
    } catch (const std::exception &e) {
        rv::log::error("Encountered exception: {}", e.what());
        rv::cleanup();
        return -1;
    }
    return 0;
}