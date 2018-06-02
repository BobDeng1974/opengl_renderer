#version 430 core


struct Light {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define N_LIGHTS 128


in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coords;

out vec4 color;

uniform sampler2D u_texture_diffuse_0;
uniform sampler2D u_texture_specular_0;

uniform vec3 u_view_pos;
uniform uint u_light_n;
uniform Light u_light[N_LIGHTS];


vec3 calc_light(Light light, vec3 normal, vec3 frag_pos, vec3 view_dir);


void main() {
    vec3 norm = normalize(normal);
    vec3 view_dir = normalize(u_view_pos - frag_pos);

    vec3 result = vec3(0.0f);

    for (int i = 0; i < u_light_n; i++) {
        result += calc_light(u_light[i], norm, frag_pos, view_dir);
    }


    color = vec4(result, 1.0f);
}

vec3 calc_light(Light light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
    vec3 light_dir = normalize(light.position - frag_pos);

    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_texture_diffuse_0, tex_coords));

    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), 45.0f);
    vec3 specular = light.specular * spec * vec3(texture(u_texture_specular_0, tex_coords));

    vec3 ambient = light.ambient * vec3(texture(u_texture_diffuse_0, tex_coords));

    float distance = length(light.position - frag_pos);

    // 1 / attenuation
    float attenuation = light.constant + light.linear * distance + light.quadratic * (distance * distance);    


    return (ambient + diffuse + specular) / attenuation;
}