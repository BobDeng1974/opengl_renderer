#include <stb/stb_image.h>

#include <algorithm>
#include <iostream>

#include "model.hpp"


namespace opengl {


Model::Model(const path& path) {
    load_model(path);
}

void Model::render(const Shader& shader) const {
    for (auto& mesh : meshes) {
        mesh.render(shader);
    }
}

void Model::load_model(const path& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.parent_path();

    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene) {
    for (std::size_t i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        meshes.push_back(process_mesh(mesh, scene));
    }

    for (std::size_t i = 0; i < node->mNumChildren; ++i) {
        process_node(node->mChildren[i], scene);
    }
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
    Vertices vertices;
    Indices indices;
    Textures textures;


    for (std::size_t i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        glm::vec3 vec;

        vec.x = mesh->mVertices[i].x;
        vec.y = mesh->mVertices[i].y;
        vec.z = mesh->mVertices[i].z;
        vertex.position = vec;
        
        vec.x = mesh->mNormals[i].x;
        vec.y = mesh->mNormals[i].y;
        vec.z = mesh->mNormals[i].z;
        vertex.normal = vec;

        if (mesh->HasTextureCoords(0)) {
            glm::vec2 vec;

            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.tex_coords = vec;
        }
        else {
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (std::size_t i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];

        for (std::size_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


    Textures diffuse_maps = load_material(material, aiTextureType_DIFFUSE, texture_type::diffuse);
    std::copy(diffuse_maps.begin(), diffuse_maps.end(), std::back_inserter(textures));

    Textures specular_maps = load_material(material, aiTextureType_SPECULAR, texture_type::specular);
    std::copy(specular_maps.begin(), specular_maps.end(), std::back_inserter(textures));

    return Mesh(vertices, indices, textures);
}

Textures Model::load_material(aiMaterial* material, aiTextureType type, texture_type t_type) {
    Textures textures_tmp;

    for (std::size_t i = 0; i < material->GetTextureCount(type); i++) {
        aiString str;
        material->GetTexture(type, (std::uint32_t)i, &str);

        bool skip = false;
        for (std::size_t j = 0; j < textures.size(); j++) {
            if (std::strcmp(textures[j].path.c_str(), str.C_Str()) == 0) {
                textures_tmp.push_back(textures[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {
            Texture texture;

            texture.id = load_texture(directory / str.C_Str());
            texture.type = t_type;
            texture.path = str.C_Str();
            textures_tmp.push_back(texture);

            textures.push_back(texture);
        }
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