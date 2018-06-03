#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <array>

#include "skulls.hpp"


namespace opengl::scenes {


struct Controller {
    struct Camera {
        glm::vec3 pos, front, up;
    };

    static Camera camera;

    static std::array<bool, 1024> keys;

    static glm::dvec2 cursor;
    static Skulls skulls;

    static void update_camera(GLfloat dt) noexcept;
    static void mouse_callback(GLFWwindow* window, double x, double y) noexcept;
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
};

} // namespace opengl::scenes