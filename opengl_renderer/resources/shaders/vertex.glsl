#version 430 core

layout(location = 0) in vec3 l_position;
layout(location = 1) in vec3 l_normal;
layout(location = 2) in vec2 l_tex_coords;

out vec3 normal;
out vec3 frag_pos;
out vec2 tex_coords;

uniform mat4 u_model;
uniform mat3 u_model_it;
uniform mat4 u_proj_view;


void main() {
    frag_pos = vec3(u_model * vec4(l_position, 1.0f));
    normal = u_model_it * l_normal;
    tex_coords = l_tex_coords;

    gl_Position = u_proj_view * vec4(frag_pos, 1.0f);
}
