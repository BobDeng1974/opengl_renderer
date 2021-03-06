#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

#include "context.hpp"
#include "lighting.hpp"
#include "model.hpp"
#include "shader.hpp"

#include "controller.hpp"


#include "debug.hpp"
#ifdef _DEBUG
    constexpr bool _debug = true;
#else
    constexpr bool _debug = false;
#endif

#include "scenes.hpp"


namespace fs = std::experimental::filesystem;
namespace gl = opengl;

using namespace std::string_literals;


int opengl::scenes::inquisitor_skull() {
    GLint width = 800;
    GLint height = 600;

    std::string title = "Inquisitor skull";

    using build = gl::release;


    auto context = gl::Context<build>(width, height, title);

    if (context.is_failed) {
        return -1;
    }

    if constexpr (context.is_debug) {
        context.set_debug_callback(gl::gl_debug_message_callback);
    }


    glfwSetKeyCallback(context.window, Controller::key_callback);

    glfwSetInputMode(context.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(context.window, &Controller::cursor.x, &Controller::cursor.y);
    glfwSetCursorPosCallback(context.window, Controller::mouse_callback);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    Controller::camera = {
        glm::vec3(.0f, .0f,  3.0f),
        glm::vec3(.0f, .0f, -1.f),
        glm::vec3(.0f, 1.f,  .0f)
    };


    const fs::path resources = "resources";

    std::shared_ptr<Shader> skull_shader = gl::Shader::create<GL_VERTEX_SHADER, GL_FRAGMENT_SHADER>(
            resources / "shaders" / "vertex.glsl",
            resources / "shaders" / "fragment.glsl"
    );

    auto lamp_shader = gl::Shader::create<GL_VERTEX_SHADER, GL_FRAGMENT_SHADER>(
            resources / "shaders" / "vertex_lamp.glsl",
            resources / "shaders" / "fragment_lamp.glsl"
    );

    
    auto inquisitor = std::make_unique<gl::Model>(
        resources / "models" / "lord_inquisitor_servo_skull" / "scene.gltf"
    );

    auto salazar = std::make_unique<gl::Model>(
        resources / "models" / "skull_salazar_downloadable" / "scene.gltf"
    );

    auto lamp = std::make_unique<gl::Model>(
        resources / "models" / "truncated_octahedron" / "scene.gltf"
    );


    skull_shader->use();


    GLfloat ligth_distance = 0.7f;

    GLfloat ambient = .05f;
    GLfloat diffuse = .8f;
    GLfloat specular = 1.f;
    GLfloat constant = 1.f;
    GLfloat linear = .09f;
    GLfloat quadratic = .032f;

    Lighting<4> lighting = { {{
            { glm::vec3(), glm::vec3(1.f, .0f, .0f), skull_shader->uniform("u_light[0].position") },
            { glm::vec3(), glm::vec3(.0f, 1.f, .0f), skull_shader->uniform("u_light[1].position") },
            { glm::vec3(), glm::vec3(.0f, .0f, 1.f), skull_shader->uniform("u_light[2].position") },
            { glm::vec3(), glm::vec3(1.f, 1.f, 1.f), skull_shader->uniform("u_light[3].position") },
        }}, 

        { constant, linear, quadratic },

        ambient,
        diffuse,
        specular
    };


    Controller::skulls.model = { std::move(inquisitor), std::move(salazar) };
    Controller::skulls.model_mat = {
        std::make_unique<glm::mat4>(
            glm::scale(glm::mat4(), glm::vec3(.01f, .01f, .01f))
        ),
        std::make_unique<glm::mat4>(
            glm::scale(glm::rotate(glm::mat4(), glm::radians(15.f), glm::vec3(1.f, .0f, .0f)), glm::vec3(.6f, .6f, .6f))
        )
    };
    Controller::skulls.model_mat_it = {
        std::make_unique<glm::mat3>(glm::transpose(glm::inverse(*Controller::skulls.model_mat[0]))),
        std::make_unique<glm::mat3>(glm::transpose(glm::inverse(*Controller::skulls.model_mat[1]))),
    };
    Controller::skulls.shader = skull_shader;
    Controller::skulls.mat_location = skull_shader->uniform("u_model");
    Controller::skulls.mat_it_location = skull_shader->uniform("u_model_it");


    glUniform1ui(skull_shader->uniform("u_light_n"), (GLuint)lighting.size);

    for (std::size_t i = 0; i < lighting.size; ++i) {
        const auto location = "u_light[" + std::to_string(i) + "]";

        glUniform3fv(skull_shader->uniform(location + ".ambient"s),   1, glm::value_ptr(lighting.lights[i].color * lighting.ambient));
        glUniform3fv(skull_shader->uniform(location + ".diffuse"s),   1, glm::value_ptr(lighting.lights[i].color * lighting.diffuse));
        glUniform3fv(skull_shader->uniform(location + ".specular"s),  1, glm::value_ptr(lighting.lights[i].color * lighting.specular));
        glUniform1f( skull_shader->uniform(location + ".constant"s),  lighting.attenuation.constant);
        glUniform1f( skull_shader->uniform(location + ".linear"s),    lighting.attenuation.linear);
        glUniform1f( skull_shader->uniform(location + ".quadratic"s), lighting.attenuation.quadratic);
    }

    glUniformMatrix4fv(skull_shader->uniform("u_model"), 1, GL_FALSE, glm::value_ptr(*Controller::skulls.model_mat[0]));
    glUniformMatrix3fv(skull_shader->uniform("u_model_it"), 1, GL_FALSE, glm::value_ptr(*Controller::skulls.model_mat_it[0]));

    const glm::mat4 projection = glm::perspective(45.f, (GLfloat)context.width / (GLfloat)context.height, .1f, 100.f);

    GLfloat shift = glm::two_pi<float>() / lighting.size;


    context.loop([&](GLfloat dt) {
        Controller::update_camera(dt);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        skull_shader->use();

        for (std::size_t i = 0; i < lighting.size; ++i) {
            lighting.lights[i].position = glm::vec3(
                glm::sin(context.current_frame * .7f + shift * i) * ligth_distance,
                .4f,
                glm::cos(context.current_frame * .7f + shift * i) * ligth_distance
            );

            glUniform3fv(lighting.lights[i].location, 1, glm::value_ptr(lighting.lights[i].position));
        }

        glm::mat4 proj_view = projection * glm::lookAt(Controller::camera.pos, Controller::camera.pos + Controller::camera.front, Controller::camera.up);

        glUniformMatrix4fv(skull_shader->uniform("u_proj_view"), 1, GL_FALSE, glm::value_ptr(proj_view));
        glUniform3fv(skull_shader->uniform("u_view_pos"), 1, glm::value_ptr(Controller::camera.pos));

        Controller::skulls.current_model()->render(*skull_shader);


        lamp_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (const auto& light : lighting.lights) {
            const glm::mat4 model = glm::scale(glm::translate(glm::mat4(), light.position), glm::vec3(.05f, .05f, .05f));

            glUniformMatrix4fv(lamp_shader->uniform("u_proj_view"), 1, GL_FALSE, glm::value_ptr(proj_view));
            glUniformMatrix4fv(lamp_shader->uniform("u_model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(lamp_shader->uniform("u_color"), 1, glm::value_ptr(light.color));

            lamp->render(*lamp_shader);
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    });


    return 0;
}