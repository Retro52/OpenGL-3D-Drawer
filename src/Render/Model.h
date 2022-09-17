#ifndef MODEL_H
#define MODEL_H

#define GLEW_STATIC
#include "../include/OpenGL/include/GLEW/glew.h"

#include "../include/OpenGL/include/glm/glm.hpp"
#include "../include/OpenGL/include/glm/gtc/matrix_transform.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "../Core/InGameException.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class Model
{
public:
    explicit Model(std::string  path);

    /**
     * Draws all model meshes
     * @param shader shader to use for drawing
     */
    void Draw(const Shader &shader, const glm::mat4& model, unsigned int shadowMap) const
    {
        shader.setMat4("model", model);
        for(const auto& mesh : meshes)
        {
            mesh.Draw(shader, shadowMap);
        }
    };

    void DrawIntoDepth(const Shader& shader, const glm::mat4& model) const
    {
        shader.setMat4("model", model);
        for(const auto& mesh : meshes)
        {
            mesh.DrawIntoDepth();
        }
    };

    /**
     * Loads textures from file
     * @param path path to the original model file
     * @param directory of the file
     * @param gamma
     * @return id of the loaded texture
     */
    static unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

    /**
     * @return model path used when loading
     */
    [[nodiscard]] inline std::string GetPath() const { return path; }
private:
    /**
     * Loads model
     * @param path path to the model
     */
    void LoadModel(std::string const &path);

    /**
     * Recursively checks all sub nodes
     * @param node current node
     * @param scene current scene
     */
    void ProcessNode(aiNode *node, const aiScene *scene);
    /**
     * Precedes mesh into given scene
     * @param mesh current mesh
     * @param scene current scene
     * @return new Mesh new loaded mesh
     */
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

    /**
     * Loads all model textures
     * @param mat material
     * @param type type of the texture
     * @param typeName name of the texture type
     * @return vector of loaded textures
     */
    std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName);
public:
    bool gammaCorrection;
    std::string directory;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
private:
    std::string path;
};
#endif
