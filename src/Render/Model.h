#ifndef MODEL_H
#define MODEL_H

#define GLEW_STATIC
#include <glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "../Core/InGameException.h"

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

class Model
{
public:
    explicit Model()
    {
        path = "";
    }

    /**
     * @param path path to the model file
     */
    explicit Model(std::string  path);

    /**
     * Draws all model meshes to the color buffer
     * @param model model model (transform) matrix
     * @param shader shader to apply
     * @param shadowMap directional light shadow map texture
     */
    void Draw(const Shader &shader, const glm::mat4& model, unsigned int shadowMap) const
    {
        shader.setMat4("model", model);
        for(const auto& mesh : meshes)
        {
            mesh->Draw(shader, shadowMap);
        }
    };

    /**
     * Draws all model meshes to the depth buffer
     * @param model model model (transform) matrix
     * @param shader shader to apply
     */
    void DrawIntoDepth(const Shader& shader, const glm::mat4& model) const
    {
        shader.setMat4("model", model);
        for(const auto& mesh : meshes)
        {
            mesh->DrawIntoDepth();
        }
    };

    void LoadModel() { LoadModel(path); }

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
    std::shared_ptr<Mesh> ProcessMesh(aiMesh *mesh, const aiScene *scene);

public:
    std::string path;

private:
    bool gammaCorrection;
    std::string directory;
    std::vector<std::shared_ptr<Mesh>> meshes;
};
#endif
