//
// Created by Anton on 28.10.2022.
//

#ifndef GRAPHICS_SSBO_HPP
#define GRAPHICS_SSBO_HPP

#define GLEW_STATIC
#include "glew.h"

#include "glm/glm.hpp"
#include <iostream>
#include <vector>

class SSBO
{
public:
    explicit SSBO(int bindingBase = 0)
    {
        glGenBuffers(1, &id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingBase, id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    /**
     * Binds current SSBO
     */
    void Bind() const
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
    }

    /**
     * Fills UBO with data of T type
     * @param data vector of data to fill
     */
    template<typename T, size_t size = 1>
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
    static void Reset()
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
private:
    unsigned int id {0};
};

#endif //GRAPHICS_SSBO_HPP
