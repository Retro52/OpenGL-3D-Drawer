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
    /* This needs some work */
    Texture() = delete;
    Texture(Texture&& ) = delete;
    Texture(const Texture& ) = delete;

    /**
     * Loads texture and creates all OpenGL needed data
     * @param path texture path
     * @param gamma gamma correction (not implemented)
     */
    Texture(const std::string& path, bool gamma = false);

    /**
     * @return texture id
     */
    [[nodiscard]] inline unsigned int GetId() const { return id; }

    /**
     * Texture comparison operator
     * @param other other texture`s path
     * @return if texture`s paths are equal
     */
    inline bool operator ==(const std::string& other) const { return std::strcmp(path.c_str(), other.c_str()) == 0; }

    /**
     * Texture comparison operator
     * @param other other texture`s path
     * @return if texture`s paths are equal
     */
    inline bool operator ==(const Texture& other) const { return std::strcmp(path.c_str(), other.path.c_str()) == 0; }

    /**
     * Texture comparison operator
     * @param other other texture`s path
     * @return if texture`s paths are equal
     */
    inline bool operator ==(const aiString& other) const { return std::strcmp(path.c_str(), other.C_Str()) == 0; }

    /**
     * Deleting texture when we are done
     */
    ~Texture();
private:
    int width = 0;
    int height = 0;
    float blend = 1.0f;
    unsigned int id = 0;
    std::string path;
};

using TextureStack = std::vector<std::shared_ptr<Texture>>;

class Material
{
public:
    /**
     * Deletes all textures, unused by materials
     */
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

    /**
     * Creates new material
     * @param material assimp loaded material
     * @param directory material directory
     */
    Material(const aiMaterial * material, const std::string& directory);

    /**
     * Binds all material textures to shader
     * @param shader draw shader
     * @param shadowTexture shadow texture, rendered by ShadowHandler
     */
    void Bind(const Shader& shader, GLuint shadowTexture) const;

private:
    /**
     * Loads all mterial textures
     * @param material assimp material
     * @param directory material texture directory
     * @param aiType texture type
     * @param texType engine texture type
     */
    void LoadTextures(const aiMaterial * material, const std::string& directory, aiTextureType aiType, TextureType texType);

    glm::vec3 defaultColor;
    std::unordered_map<TextureType, TextureStack> materialTextures;

    static std::vector<std::shared_ptr<Texture>> texturesLoaded;

    float opacity;
    float tilingFactor = 1.0f;
    bool isTwoSided = false;
};
#endif //GRAPHICS_MATERIAL_H
