#include <stb/stb_image.h>

#include <algorithm>
#include <iostream>
#include <utility>

#include "model.hpp"


namespace opengl {

Model::Model(const path& path) :
    Model(std::forward<decltype(path)>(path)) {}

Model::Model(path&& path) : directory(path.parent_path()) {
    load_model(std::forward<decltype(path)>(path));
}

void Model::render(const Shader& shader) const {
    for (const auto& mesh : meshes) {
        mesh.render(shader);
    }
}

void Model::load_model(path&& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene) {
    for (std::size_t i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        meshes.push_back(std::move(process_mesh(mesh, scene)));
    }

    for (std::size_t i = 0; i < node->mNumChildren; ++i) {
        process_node(node->mChildren[i], scene);
    }
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
    Vertices vertices;
    vertices.reserve(mesh->mNumVertices);

    for (std::size_t i = 0; i < mesh->mNumVertices; ++i) {
        const Vertex vertex = { {
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            }, {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            },
            mesh->HasTextureCoords(0)
            ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
            : glm::vec2()
        };

        vertices.push_back(std::move(vertex));
    }


    Indices indices;

    for (std::size_t i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];

        for (std::size_t j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


    Textures textures;
    Textures diffuse_maps = load_material(material, aiTextureType_DIFFUSE, texture_type::diffuse);
    Textures specular_maps = load_material(material, aiTextureType_SPECULAR, texture_type::specular);

    textures.swap(diffuse_maps);
    std::copy(specular_maps.begin(), specular_maps.end(), std::back_inserter(textures));

    return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

Textures Model::load_material(aiMaterial* material, aiTextureType type, texture_type t_type) {
    Textures textures_tmp;

    for (std::size_t i = 0; i < material->GetTextureCount(type); ++i) {
        aiString str;
        material->GetTexture(type, (std::uint32_t)i, &str);

        Texture texture;
        if (auto&& it = textures.find(str.C_Str()); it == textures.end()) {
            texture.id = load_texture(directory / str.C_Str());
            texture.type = t_type;
            texture.uniform = [&] {
                switch (t_type) {
                case texture_type::specular:
                    return "u_texture_specular_" + std::to_string(specular_n++); break;
                case texture_type::diffuse: [[fallthrough]];
                default:
                    return "u_texture_diffuse_" + std::to_string(diffuse_n++); break;
                }
            }();

            textures[str.C_Str()] = texture;
        }
        else {
            texture = it->second;
        }

        textures_tmp.push_back(std::move(texture));
    }


    return textures_tmp;
}


GLuint load_texture(const std::experimental::filesystem::path& path) {
    unsigned int id;
    glGenTextures(1, &id);

    int width, height, nr;
    auto* data = stbi_load(path.string().c_str(), &width, &height, &nr, 0);

    if (data) {
        GLenum format;
        switch (nr) {
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        case 1: [[fallthrough]];
        default: format = GL_RED; break;
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }

    return id;
}

} // namespace opengl