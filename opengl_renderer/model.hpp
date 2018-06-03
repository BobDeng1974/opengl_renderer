#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>

#include "mesh.hpp"
#include "primitives.hpp"
#include "shader.hpp"


namespace opengl {


class Model {
public:
    using path = std::experimental::filesystem::path;

    Model(const path& path);
    Model(path&& path);

    void render(const Shader& shader) const;
    const path directory;

private:
    void load_model(path&& path);

    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);

    Textures load_material(aiMaterial* material, aiTextureType type, texture_type t_type);

    TextureCache textures;
    Meshes meshes;

    std::size_t diffuse_n = 0;
    std::size_t specular_n = 0;
};

GLuint load_texture(const std::experimental::filesystem::path& path);

} // namespace opengl