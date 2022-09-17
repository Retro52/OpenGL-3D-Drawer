//
// Created by Anton on 15.09.2022.
//

#ifndef GRAPHICS_FBO_HPP
#define GRAPHICS_FBO_HPP

#define GLEW_STATIC
#include "../vendors/include/GLEW/glew.h"
#include "../vendors/include/glm/glm.hpp"
#include <iostream>
#include <vector>

class FBO
{
public:
    FBO()
    {
        glGenFramebuffers(1, &id);
    }

    ~FBO()
    {
        glDeleteFramebuffers(1, &id);
    }

    [[nodiscard]] inline unsigned int Get() const { return id; }

    inline void AddTexture(unsigned int texture, unsigned int attachmentType) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    inline void SetDrawBuffer(unsigned int mode) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glDrawBuffer(mode);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    inline void SetReadBuffer(unsigned int mode) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glReadBuffer(mode);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    inline void Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }

    inline void Reset() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

private:
    unsigned int id { 0 };
};

#endif //GRAPHICS_FBO_HPP
