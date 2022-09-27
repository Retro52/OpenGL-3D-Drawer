//
// Created by Anton on 18.09.2022.
//

#ifndef GRAPHICS_MATERIAL_H
#define GRAPHICS_MATERIAL_H

#define GLEW_STATIC

#include "../vendors/include/GLEW/glew.h"
#include "../Logging/easylogging++.h"
#include "Shader.h"

#include <iostream>
#include <vector>
#include <array>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


using TextureType = enum TextureType
{
    Diffuse,
    Normal,
    Specular,
    LightMap,
    Roughness,
    Translucency
};

struct Texture
{
    Texture() = delete;
    Texture(Texture&& ) = delete;
    Texture(const Texture& ) = delete;

    Texture(const std::string& path, bool gamma = false);

    [[nodiscard]] inline unsigned int GetId() const { return id; }

    inline bool operator ==(const std::string& other) { return std::strcmp(path.c_str(), other.c_str()) == 0; }

    inline bool operator ==(const Texture& other) { return std::strcmp(path.c_str(), other.path.c_str()) == 0; }

    inline bool operator ==(const aiString& other) { return std::strcmp(path.c_str(), other.C_Str()) == 0; }

    /**
     * Deleting texture when we are done
     */
    ~Texture();
    std::string path;

private:
    int width = 0;
    int height = 0;
    float blend = 1.0f;
    unsigned int id = 0;

//    std::string path;
};

using TextureStack = std::vector<std::shared_ptr<Texture>>;

class Material
{
public:
    /* Works good enough, but sometimes skips some textures for no obvious reason */
    static void UnloadUnusedTextures()
    {
        for (auto it = texturesLoaded.begin(); it != texturesLoaded.end();)
        {
            if ((* it).use_count() == 1)
            {
                it = texturesLoaded.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    ~Material() = default;

    Material(const aiMaterial * material, const std::string& directory);

    void Bind(const Shader& shader, GLuint shadowTexture) const;

private:
    void LoadTextures(const aiMaterial * material, const std::string& directory, aiTextureType aiType, TextureType texType);

    glm::vec3 defaultColor;
    std::unordered_map<TextureType, TextureStack> materialTextures;

    static std::vector<std::shared_ptr<Texture>> texturesLoaded;

    float opacity;
    float tilingFactor = 1.0f;
    bool isTwoSided = false;
};
#endif //GRAPHICS_MATERIAL_H
