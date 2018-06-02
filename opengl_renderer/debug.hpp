#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace opengl {


void APIENTRY gl_debug_message_callback(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data);

} // namespace opengl