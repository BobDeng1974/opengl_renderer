#version 430 core

in vec2 tex_coords;

out vec4 color;

uniform sampler2D u_texture_diffuse_0;


void main() {
    color = vec4(texture(u_texture_diffuse_0, tex_coords).rgb, 1.0f);
}