//
// Created by Anton on 18.09.2022.
//

#ifndef GRAPHICS_MATERIAL_H
#define GRAPHICS_MATERIAL_H

#define GLEW_STATIC

#include "glew.h"
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
    explicit Texture(const std::string& path, bool gamma = false);

    /**
     * Creates an empty texture
     * @param width texture width
     * @param height texture height
     * @param format texture format, GL_RGBA by default
     * @param internalFormat texture internal format, GL_RGBA by default
     * @param repeat if texture should be repeated or clamped to the border, true by default
     * @param isTextureArray true if required texture has to be TEXTURE_2D_ARRAY, false by default
     * @param textureArraySize texture array size, use only if texture is texture 2D array, -1 by default
     */
    Texture(GLsizei width, GLsizei height, unsigned int format = GL_RGBA, unsigned int internalFormat = GL_RGBA, unsigned int pixelType = GL_UNSIGNED_BYTE, bool repeat = true, bool isTextureArray = false, unsigned int textureArraySize = 0);

    /**
     * @return texture id
     */
    [[nodiscard]] inline unsigned int GetId() const { return id; }

    void Bind() const { glBindTexture(textureType, id); }

    [[nodiscard]] std::string GetPath() const
    {
        return path;
    }
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
public:
    static std::vector<std::shared_ptr<Texture>> texturesLoaded;

private:
    int width = 0;
    int height = 0;
    float blend = 1.0f;
    unsigned int id = 0;
    unsigned int textureType = 0;
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
        for (auto it = Texture::texturesLoaded.begin(); it != Texture::texturesLoaded.end();)
        {
            if ((* it).use_count() == 1)
            {
                it = Texture::texturesLoaded.erase(it);
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
    void Bind(const std::shared_ptr<Shader>& shader) const;

public:
    float opacity;
    float specular;
    glm::vec3 defaultColor;
    std::unordered_map<TextureType, TextureStack> materialTextures;

private:
    /**
     * Loads all material textures
     * @param material assimp material
     * @param directory material texture directory
     * @param aiType texture type
     * @param texType engine texture type
     */
    void LoadTextures(const aiMaterial * material, const std::string& directory, aiTextureType aiType, TextureType texType);
    bool isTwoSided = false;
};
#endif //GRAPHICS_MATERIAL_H
