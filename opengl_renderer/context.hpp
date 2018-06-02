#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <string>
#include <type_traits>


namespace opengl {


struct debug {};
struct release {};

template <class Build = void>
struct Context {
    static_assert(
        std::is_same_v<Build, debug> || std::is_same_v<Build, release>,
        "You must specify build type!"
    );

    static constexpr const bool is_debug = std::is_same_v<Build, debug>;


    GLFWwindow* window;
    const GLint width, height;

    bool is_failed = false;

    GLfloat last_frame;


    Context(GLint width, GLint height, const std::string& title) : width(width), height(height) {
        if (!glfwInit()) {
            is_failed = true; return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if constexpr (is_debug) {
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        }

        window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (window == nullptr) {
            glfwTerminate();

            is_failed = true; return;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            is_failed = true; return;
        }

        GLint window_width, window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);

        glViewport(0, 0, window_width, window_height);
        glEnable(GL_DEPTH_TEST);

        if constexpr (is_debug) {
            std::cout << glGetString(GL_VERSION) << std::endl;
        }

        last_frame = (GLfloat)glfwGetTime();
    }

    virtual ~Context() {
        glfwTerminate();
    }


    template <
        class Callback,
        class = std::enable_if<is_debug>
    >
    void set_debug_callback(Callback message_callback) {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(message_callback, nullptr);
        glDebugMessageControl(
            GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            nullptr,
            GL_TRUE
        );

        glEnable(GL_DEBUG_OUTPUT);
    }


    void loop(std::function<void(GLfloat)> loop_callback) {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            GLfloat current_frame = (GLfloat)glfwGetTime();
            GLfloat dt = current_frame - last_frame;
            last_frame = current_frame;

            loop_callback(dt);

            glfwSwapBuffers(window);
        }
    }
};


}  // namespace opengl