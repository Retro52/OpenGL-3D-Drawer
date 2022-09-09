//
// Created by Anton on 09.09.2022.
//

#ifndef GRAPHICS_UBO_H
#define GRAPHICS_UBO_H

#define GLEW_STATIC
#include "../include/OpenGL/include/GLEW/glew.h"
#include "../include/OpenGL/include/glm/glm.hpp"

template<typename T, size_t size>
class UBO
{
    UBO()
    {
        glGenBuffers(1, &matricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(T) * size, nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    ~UBO()
    {
        unsigned int lst[] = { matricesUBO };
        glDeleteBuffers(1, lst);
    }
private:
    unsigned int matricesUBO {0};
};


#endif //GRAPHICS_UBO_H
