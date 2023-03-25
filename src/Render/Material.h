//
// Created by Anton on 18.09.2022.
//

#ifndef GRAPHICS_MATERIAL_H
#define GRAPHICS_MATERIAL_H

#define GLEW_STATIC

#include "glew.h"
#include "../Logging/easylogging++.h"
#include "../Core/EngineException.h"
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
    Metallic,
    Roughness,
    Translucency
};

struct Texture
{
public:
    Texture(Texture&& ) = default;
    Texture(const Texture& ) = default;

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
     * @param pixelType texture pixel tipe, GL_UNSIGNED_BYTE by default
     * @param repeat if texture should be repeated or clamped to the border, true by default
     */
    Texture(GLsizei width, GLsizei height, unsigned int format = GL_RGBA, unsigned int internalFormat = GL_RGBA, unsigned int pixelType = GL_UNSIGNED_BYTE, bool repeat = true);

    /**
     * A simple wrapper to create a texture not using constructor
     * @param width texture width
     * @param height texture height
     * @param format texture format, GL_RGBA by default
     * @param internalFormat texture internal format, GL_RGBA by default
     * @param pixelType texture pixel tipe, GL_UNSIGNED_BYTE by default
     * @param repeat if texture should be repeated or clamped to the border, true by default
     * @return created texture
     */
    template<typename... Args>
    static Texture CreateTexture(Args&&... args)
    {
        return Texture(std::forward<Args>(args)...);
    }

    static std::shared_ptr<Texture> CreateTextureArray(GLsizei width, GLsizei height, unsigned int textureArraySize = 0, unsigned int format = GL_RGBA, unsigned int internalFormat = GL_RGBA, unsigned int pixelType = GL_UNSIGNED_BYTE, bool repeat = true);

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
private:
    Texture() = default;
public:
    inline static std::vector<std::shared_ptr<Texture>> texturesLoaded;

private:
    int width = 0;
    int height = 0;
    int samples = 0;
    float blend = 1.0f;
    unsigned int id = 0;
    unsigned int textureType = 0;
    std::string path;
};

struct CubeMap
{
    /**
     * Correct order of faces is: right, left, top, bottom, front, back
     * @param faces an array of path's to the textures
     */
    explicit CubeMap(const std::array<std::string, 6>& faces = std::array<std::string, 6>());
    ~CubeMap() = default;

    void Bind() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    }

    [[nodiscard]] unsigned int GetId() const
    {
        return id;
    }

private:
    unsigned int id;
};


using TextureStack = std::vector<std::shared_ptr<Texture>>;

class Material
{
public:
    /**
    * Loads texture at given path or returns an already loaded one if paths are the same
    * @param path texture path
    * @return pointer to the texture
    */
    static std::shared_ptr<Texture> LoadTexture(const std::string& path);

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

    Material() = default;
    ~Material() = default;

    /**
     * Loads material file
     * @param path path to the file
     */
    Material(const std::string& path);

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

    void AttachTexture(const std::shared_ptr<Texture>& texture, TextureType type)
    {
        materialTextures[type].push_back(texture);
    }

public:
    float specular = 0.0f;
    float metallic = 0.0f;
    float roughness = 1.0f;
    glm::vec4 defaultColor;
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
