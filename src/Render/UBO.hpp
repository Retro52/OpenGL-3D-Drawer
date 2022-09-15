//
// Created by Anton on 09.09.2022.
//

#ifndef GRAPHICS_UBO_HPP
#define GRAPHICS_UBO_HPP

#define GLEW_STATIC
#include "../include/OpenGL/include/GLEW/glew.h"
#include "../include/OpenGL/include/glm/glm.hpp"
#include <iostream>
#include <vector>

template<typename T, size_t size>
class UBO
{
public:
    /**
     * Creates empty UBO
     */
    UBO()
    {
        glGenBuffers(1, &id);
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(T) * size, nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, id);
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
    void FillData(const std::vector<T>& data) const
    {
        size_t optSize = data.size() < size ? data.size() : size;
        for (size_t i = 0; i < optSize; ++i)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(T), sizeof(T), &data[i]);
        }
    }

    /**
     * Binds UBO to 0
     */
    void Reset()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    /**
     * Deletes buffer from OpenGL
     */
    ~UBO()
    {
        unsigned int lst[] = { id };
        glDeleteBuffers(1, lst);
    }
private:
    unsigned int id {0};
};


#endif //GRAPHICS_UBO_HPP
