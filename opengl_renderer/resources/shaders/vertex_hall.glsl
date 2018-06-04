#version 430 core

layout(location = 0) in vec3 l_position;
layout(location = 1) in vec3 l_normal;
layout(location = 2) in vec2 l_tex_coords;

out vec2 tex_coords;

uniform mat4 u_model;
uniform mat4 u_proj_view;


void main() {
    vec3 frag_pos = vec3(u_model * vec4(l_position, 1.0f));
    tex_coords = l_tex_coords;

    gl_Position = u_proj_view * vec4(frag_pos, 1.0f);
}
