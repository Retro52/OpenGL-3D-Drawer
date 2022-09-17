#ifndef MESH_H
#define MESH_H

#define GLEW_STATIC

#include "../vendors/include/GLEW/glew.h"
#include "../vendors/include/glm/glm.hpp"

#include "Shader.h"

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

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    /***
     * Mesh constructor, creates new instance of the mesh
     * @param vertices vector of mesh vertices
     * @param indices  vector of mesh indices
     * @param textures vector of mesh textures
     */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    /***
     * Draws mesh instance
     * @param shader shader to be applied to the instance when drawing
     */
    void Draw(const Shader &shader, GLuint shadowMap) const;

    void DrawIntoDepth() const;
private:
    unsigned int VBO{}, EBO{};

    unsigned int VAO{};
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    /***
     * Initializes buffers and textures for OpenGL
     */
    void SetUpMesh();
};
#endif
