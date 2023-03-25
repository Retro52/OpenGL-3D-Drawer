#ifndef MESH_H
#define MESH_H

#define GLEW_STATIC

#include "glew.h"
#include "glm/glm.hpp"

#include "Shader.h"
#include "Material.h"
#include "../Core/Profiler.hpp"

#include <string>
#include <vector>

constexpr int MAX_BONE_INFLUENCE = 4;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

class Mesh
{
public:

    Mesh(Mesh&& other) = delete;
    Mesh(const Mesh& other) = delete;

    /***
     * Mesh constructor, creates new instance of the mesh
     * @param vertices vector of mesh vertices
     * @param indices  vector of mesh indices
     * @param textures vector of mesh textures
     */
    Mesh(std::vector<Vertex>  vertices, std::vector<unsigned int>  indices, const Material& material, std::string name = "No name");

    /* Cleaning OpenGL stuff */
    ~Mesh()
    {
        Profiler::totalMeshes--;
        Profiler::totalVertices -= vertices.size();

        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }
    /***
     * Draws mesh instance
     * @param shader shader to be applied to the instance when drawing
     */
    void Draw(const std::shared_ptr<Shader> &shader) const;

    /**
     * Draws mesh, without applying any textures, useful for depth buffer draw
     */
    void DrawIntoDepth() const;
public:
    std::string name;
    Material material;
    bool overrideDefaultMaterial = true;

private:
    unsigned int VBO{}, EBO{};
    unsigned int VAO{};
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    /***
     * Initializes buffers and textures for OpenGL
     */
    void SetUpMesh();
};
#endif
