//
// Created by Anton on 18.09.2022.
//

#include <iomanip>
#include "Material.h"
#include "../Core/InGameException.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// stores all loaded textures
std::vector<std::shared_ptr<Texture>> Material::texturesLoaded;

Texture::Texture(const std::string &path, bool /*gamma*/) : path(path)
{
    std::string filename = std::quoted(path)._M_string;
    stbi_set_flip_vertically_on_load(0);

    int nrComponents = 0;

    unsigned char * data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

    GAME_ASSERT(data != nullptr, "Failed to load texture " + path + "Reason: " + std::string(stbi_failure_reason()));

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
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}

Texture::~Texture()
{
//    LOG(DEBUG) << "Texture unloaded: " << path  << "\n";
    glDeleteTextures(1, &id);
}

Material::Material(const aiMaterial *material, const std::string &directory)
{
    aiColor4D diffuse;
    if(aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS)
    {
        defaultColor = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
    }

    if (material->Get(AI_MATKEY_OPACITY, opacity) != AI_SUCCESS)
    {
        opacity = 1.0f;
    }

    LoadTextures(material, directory, aiTextureType_NORMALS, Normal);
    LoadTextures(material, directory, aiTextureType_DIFFUSE, Diffuse);
    LoadTextures(material, directory, aiTextureType_SPECULAR, Specular);
    LoadTextures(material, directory, aiTextureType_LIGHTMAP, LightMap);
    LoadTextures(material, directory, aiTextureType_SHININESS, Roughness);
    LoadTextures(material, directory, aiTextureType_OPACITY, Translucency);
}

void Material::Bind(const Shader& shader, GLuint shadowTexture) const
{
    // bind appropriate textures
    unsigned int texturesCount = 0;
    shader.setVec3("material.colorDiffuse", defaultColor);
    shader.setFloat("material.opacity", opacity);
    shader.setFloat("material.tilingFactor", tilingFactor);

    if (materialTextures.at(Diffuse).empty())
    {
        shader.setBool("material.hasDiffuseTexture", false);
        glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
        shader.setInt("material.mapDiffuse_1", (int) texturesCount++);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        shader.setBool("material.hasDiffuseTexture", true);
        for(const auto& texture : materialTextures.at(Diffuse))
        {
            unsigned int count = 1;
            glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
            shader.setInt("material.mapDiffuse_" + std::to_string(count++), (int) texturesCount++);
            glBindTexture(GL_TEXTURE_2D, texture->GetId());
        }
    }

    if (materialTextures.at(Normal).empty())
    {
        shader.setBool("material.hasNormalTexture", false);
        glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
        shader.setInt("material.mapNormal_1", (int) texturesCount++);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        shader.setBool("material.hasNormalTexture", true);
        for(const auto& texture : materialTextures.at(Normal))
        {
            unsigned int count = 1;
            glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
            shader.setInt("material.mapNormal_" + std::to_string(count++), (int) texturesCount++);
            glBindTexture(GL_TEXTURE_2D, texture->GetId());
        }
    }

    if (materialTextures.at(Specular).empty())
    {
        glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
        shader.setInt("material.mapSpecular_1", (int) texturesCount++);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        for(const auto& texture : materialTextures.at(Specular))
        {
            unsigned int count = 1;
            glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
            shader.setInt("material.mapSpecular_" + std::to_string(count++), (int) texturesCount++);
            glBindTexture(GL_TEXTURE_2D, texture->GetId());
        }
    }
    if (materialTextures.at(Roughness).empty())
    {
        glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
        shader.setInt("material.mapRoughness_1", (int) texturesCount++);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        for(const auto& texture : materialTextures.at(Roughness))
        {
            unsigned int count = 1;
            glActiveTexture(GL_TEXTURE0 + texturesCount); // active proper texture unit before binding
            shader.setInt("material.mapRoughness_" + std::to_string(count++), (int) texturesCount++);
            glBindTexture(GL_TEXTURE_2D, texture->GetId());
        }
    }

    glActiveTexture(GL_TEXTURE0 + texturesCount);
    shader.setInt("material.mapShadow", (int) texturesCount);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowTexture);
}

void Material::LoadTextures(const aiMaterial *material, const std::string &directory, aiTextureType aiType, TextureType texType)
{
    std::vector<Texture> textures_loaded;

    materialTextures[texType];

    for(unsigned int i = 0; i < material->GetTextureCount(aiType); i++)
    {
        aiString path;
        material->GetTexture(aiType, i, &path);

        std::string filename;
        if (path.C_Str()[0] != '/' && directory[directory.length() - 1] != '/')
        {
            filename = directory + '/' + path.C_Str();
        }
        else
        {
            filename = directory + path.C_Str();
        }

        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(auto & j : texturesLoaded)
        {
            if(* j == filename)
            {
                materialTextures[texType].push_back(j);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one
                break;
            }
        }
        if(!skip)
        {
            std::shared_ptr<Texture> texture = std::make_shared<Texture>(filename);
            materialTextures[texType].push_back(texture);
            texturesLoaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessarily load duplicate textures.
        }
    }
}