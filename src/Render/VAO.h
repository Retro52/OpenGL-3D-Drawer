//
// Created by Anton on 14.09.2022.
//

/**
 * I will complete it later
 */

#ifndef GRAPHICS_VAO_H
#define GRAPHICS_VAO_H

#define GLEW_STATIC

#include "../include/OpenGL/include/GLEW/glew.h"
#include "../include/OpenGL/include/glm/glm.hpp"

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

class VAO
{
    VAO() : id(0)
    {
        glGenVertexArrays(1, &id);
    }

    ~VAO() = default;

    void Bind() const
    {
        glBindVertexArray(id);
    }
    void Reset() const
    {
        glBindVertexArray(0);
    }
private:
    unsigned int id;
};


#endif //GRAPHICS_VAO_H
