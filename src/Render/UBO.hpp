//
// Created by Anton on 09.09.2022.
//

#ifndef GRAPHICS_UBO_HPP
#define GRAPHICS_UBO_HPP

#define GLEW_STATIC
#include "glew.h"

#include "glm/glm.hpp"
#include <iostream>
#include <vector>

template<typename T, size_t size>
class UBO
{
public:
    /**
     * Creates empty UBO
     */
    explicit UBO(int bindingBase = 0)
    {
        glGenBuffers(1, &id);
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(T) * size, nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingBase, id);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }


    /**
     * Binds this UBO
     */
    void Bind() const
    {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
    }

    /**
     * Fills UBO with data of T type
     * @param data vector of data to fill
     */
//    template<typename T, size_t size = 1>
    void FillData(const std::vector<T>& data, int offset = 0) const
    {
        size_t optSize = data.size() < size ? data.size() : size;
        for (size_t i = 0 + offset; i < optSize; ++i)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(T), sizeof(T), &data.at(i));
        }
    }

    /**
     * Binds UBO to 0
     */
    void Reset() const
    {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    /**
     * Deletes buffer from OpenGL
     */
    ~UBO()
    {
        glDeleteBuffers(1, &id);
    }
private:
    unsigned int id {0};
};


#endif //GRAPHICS_UBO_HPP
