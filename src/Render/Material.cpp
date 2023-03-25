//
// Created by Anton on 18.09.2022.
//
#include <iostream>
#include <mutex>
#include <thread>
#include <future>
#include "json.hpp"
#include <functional>
#include "Material.h"
#include "../Core/EngineException.h"
#include "../Core/Profiler.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Texture::Texture(const std::string& path, bool /*gamma*/) : path(path)
{
    textureType = GL_TEXTURE_2D;

    stbi_set_flip_vertically_on_load(0);

    int nrComponents = 0;

    unsigned char * data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

    ASSERT(data != nullptr, "Failed to load texture " + path + "Reason: " + std::string(stbi_failure_reason() == nullptr ? "" : stbi_failure_reason()));

    GLenum format = 0;
    if (nrComponents == 1)
    {
        format = GL_RED;
    }
    else if (nrComponents == 3)
    {
        format = GL_RGB;
    }
    else if (nrComponents == 4)
    {
        format = GL_RGBA;
    }

    glGenTextures(1, &id);
    glBindTexture(textureType, id);

    glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(textureType, 0, static_cast<int>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(textureType);

    LOG(INFO) << "Texture " << path << " loaded, taking " << nrComponents * width * height << " bytes of memory; Generated id: " << id;
    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

Texture::Texture(GLsizei width, GLsizei height, unsigned int format, unsigned int internalFormat, unsigned int pixelType, bool repeat)
{
    textureType = GL_TEXTURE_2D;

    glGenTextures(1, &id);
    glBindTexture(textureType, id);

    glTexImage2D(textureType, 0, static_cast<GLsizei>(internalFormat), width, height, 0, format, pixelType, nullptr);


    glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (!repeat)
    {
        glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, bordercolor);
    }
    else
    {
        glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    glGenerateMipmap(textureType);
}

std::shared_ptr<Texture> Texture::CreateTextureArray(GLsizei width, GLsizei height, unsigned int textureArraySize, unsigned int format, unsigned int internalFormat, unsigned int pixelType, bool repeat)
{
    std::shared_ptr<Texture> temp (new Texture());

    temp->textureType = GL_TEXTURE_2D_ARRAY;

    glGenTextures(1, &temp->id);
    glBindTexture(temp->textureType, temp->id);

    ASSERT(textureArraySize > 0, "Incorrect texture array size, must be over 0");
    glTexImage3D(
            temp->textureType, 0, static_cast<GLsizei>(internalFormat), width, height, static_cast<GLsizei>(textureArraySize) + 1,
            0, format, GL_FLOAT, nullptr);
    glGenerateMipmap(temp->textureType);

    glTexParameteri(temp->textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(temp->textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (!repeat)
    {
        glTexParameteri(temp->textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(temp->textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(temp->textureType, GL_TEXTURE_BORDER_COLOR, bordercolor);
    }
    else
    {
        glTexParameteri(temp->textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(temp->textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    return temp;
}

Material::Material(const aiMaterial *material, const std::string &directory)
{
    aiColor4D diffuse;
    if(aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS)
    {
        defaultColor = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
    }
    if (material->Get(AI_MATKEY_SPECULAR_FACTOR, specular) != AI_SUCCESS)
    {
        specular = 0.0f;
    }

    LoadTextures(material, directory, aiTextureType_NORMALS, Normal);
    LoadTextures(material, directory, aiTextureType_DIFFUSE, Diffuse);
    LoadTextures(material, directory, aiTextureType_SPECULAR, Specular);
    LoadTextures(material, directory, aiTextureType_METALNESS, Metallic);
    LoadTextures(material, directory, aiTextureType_DIFFUSE_ROUGHNESS, Roughness);
}

void Material::Bind(const std::shared_ptr<Shader>& shader) const
{
    // bind appropriate textures
    unsigned int texturesCount = 0;
    shader->setFloat("material.specular", specular);
    shader->setFloat("material.metallic", metallic);
    shader->setFloat("material.roughness", roughness);
    shader->setVec4("material.colorDiffuse", defaultColor);

    if (materialTextures.at(Diffuse).empty())
    {
        shader->setBool("material.hasDiffuseTexture", false);
        glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
        shader->setInt("material.mapDiffuse_1", (int) texturesCount++);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        shader->setBool("material.hasDiffuseTexture", true);
        for(const auto& texture : materialTextures.at(Diffuse))
        {
            unsigned int count = 1;
            glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
            shader->setInt("material.mapDiffuse_" + std::to_string(count++), (int) texturesCount++);
            glBindTexture(GL_TEXTURE_2D, texture->GetId());
        }
    }

    if (materialTextures.at(Normal).empty())
    {
        shader->setBool("material.hasNormalTexture", false);
        glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
        shader->setInt("material.mapNormal_1", (int) texturesCount++);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        shader->setBool("material.hasNormalTexture", true);
        for(const auto& texture : materialTextures.at(Normal))
        {
            unsigned int count = 1;
            glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
            shader->setInt("material.mapNormal_" + std::to_string(count++), (int) texturesCount++);
            glBindTexture(GL_TEXTURE_2D, texture->GetId());
        }
    }

    if (materialTextures.at(Specular).empty())
    {
        shader->setBool("material.hasSpecularTexture", false);
        glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
        shader->setInt("material.mapSpecular_1", (int) texturesCount++);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        shader->setBool("material.hasSpecularTexture", true);
        for(const auto& texture : materialTextures.at(Specular))
        {
            unsigned int count = 1;
            glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
            shader->setInt("material.mapSpecular_" + std::to_string(count++), (int) texturesCount++);
            glBindTexture(GL_TEXTURE_2D, texture->GetId());
        }
    }
    if (materialTextures.at(Roughness).empty())
    {
        shader->setBool("material.hasRoughnessTexture", false);
        glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
        shader->setInt("material.mapRoughness_1", (int) texturesCount++);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        shader->setBool("material.hasRoughnessTexture", true);
        for(const auto& texture : materialTextures.at(Roughness))
        {
            unsigned int count = 1;
            glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
            shader->setInt("material.mapRoughness_" + std::to_string(count++), (int) texturesCount++);
            glBindTexture(GL_TEXTURE_2D, texture->GetId());
        }
    }

    if (materialTextures.at(Metallic).empty())
    {
        shader->setBool("material.hasMetallicTexture", false);
        glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
        shader->setInt("material.mapMetallic_1", (int) texturesCount++);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        shader->setBool("material.hasMetallicTexture", true);
        for(const auto& texture : materialTextures.at(Metallic))
        {
            unsigned int count = 1;
            glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
            shader->setInt("material.mapMetallic_" + std::to_string(count++), (int) texturesCount++);
            glBindTexture(GL_TEXTURE_2D, texture->GetId());
        }
    }
}

std::shared_ptr<Texture> Material::LoadTexture(const std::string &path)
{
    // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
    for(auto & j : Texture::texturesLoaded)
    {
        if(* j == path)
        {
            return j;
        }
    }
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(path);
    Texture::texturesLoaded.push_back(texture);
    return texture;
}

void Material::LoadTextures(const aiMaterial *material, const std::string &directory, aiTextureType aiType, TextureType texType)
{
    std::vector<Texture> textures_loaded;

    materialTextures[texType];

    for(unsigned int i = 0; i < material->GetTextureCount(aiType); i++)
    {
        aiString path;
        material->GetTexture(aiType, i, &path);

        // TODO: std::filesystem might be a better option, but this works fine, at least so far
        std::string filename;
        if (path.C_Str()[0] != '/' && directory[directory.length() - 1] != '/' &&
            path.C_Str()[0] != '\\' && directory[directory.length() - 1] != '\\')
        {
            filename = directory + '\\' + path.C_Str();
        }
        else
        {
            filename = directory + path.C_Str();
        }

        materialTextures[texType].push_back(LoadTexture(filename));
    }
}

CubeMap::CubeMap(const std::array<std::string, 6>& faces) : id(0)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    if(!faces.empty())
    {
        int width, height, nrComponents;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);

            GLenum format = 0;
            GLenum internalFormat = 0;
            if (nrComponents == 1)
            {
                format = GL_RED;
                internalFormat = GL_R8;
            }
            else if (nrComponents == 3)
            {
                format = GL_RGB;
                internalFormat = GL_RGB8;
            }
            else if (nrComponents == 4)
            {
                format = GL_RGBA;
                internalFormat = GL_RGBA8;
            }

            if (data)
            {
                LOG(INFO) << "Texture " << faces[i] << " loaded, taking " << nrComponents * width * height << " bytes of memory, number of components: " << nrComponents;

                auto textureType = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

                glTexImage2D(textureType, 0, static_cast<int>(internalFormat), width, height, 0, format, GL_UNSIGNED_BYTE, data);

                glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }
            else
            {
                LOG(WARNING) << "CubeMap texture failed to load at path: " << faces[i];
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
}
