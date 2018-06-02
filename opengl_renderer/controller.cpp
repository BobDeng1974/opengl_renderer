#include <glm/gtc/type_ptr.hpp>

#include "controller.hpp"


namespace opengl::scenes {


Controller::Camera Controller::camera;
std::array<bool, 1024> Controller::keys;
glm::dvec2 Controller::cursor;
Skulls Controller::skulls;


void Controller::update_camera(GLfloat dt) {
    GLfloat speed = 1.f * dt;

    if (keys[GLFW_KEY_W]) { camera.pos += speed * camera.front; }
    if (keys[GLFW_KEY_S]) { camera.pos -= speed * camera.front; }
    if (keys[GLFW_KEY_A]) { camera.pos -= glm::normalize(glm::cross(camera.front, camera.up)) * speed; }
    if (keys[GLFW_KEY_D]) { camera.pos += glm::normalize(glm::cross(camera.front, camera.up)) * speed; }
}


void Controller::mouse_callback(GLFWwindow * window, double x, double y) {
    static GLdouble yaw = -glm::half_pi<float>();
    static GLdouble pitch = .0f;

    glm::dvec2 shift = {
        x - cursor.x,
        cursor.y - y
    };

    cursor = { x, y };

    GLdouble sensetivity = .005f;
    shift *= sensetivity;

    yaw += shift.x;
    pitch += shift.y;

    pitch = glm::clamp(pitch, -glm::half_pi<double>() + glm::radians(5.f), glm::half_pi<double>() - glm::radians(5.f));

    glm::vec3 front = {
        cos(yaw) * cos(pitch),
        sin(pitch),
        sin(yaw) * cos(pitch)
    };

    camera.front = glm::normalize(front);
}


void Controller::key_callback(GLFWwindow * window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        skulls.swap();
    }
    else if (key >= 0 && key < 1024) {
        switch (action) {
        case GLFW_PRESS:   keys[key] = true;  break;
        case GLFW_RELEASE: keys[key] = false; break;
        }
    }
}

} // namespace opengl::scenes