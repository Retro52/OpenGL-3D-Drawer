//
// Created by Anton on 15.09.2022.
//

#ifndef GRAPHICS_FBO_HPP
#define GRAPHICS_FBO_HPP

#define GLEW_STATIC
#include "glew.h"
#include "glm/glm.hpp"
#include <iostream>
#include <vector>

class FBO
{
public:
    /**
     * Creates new FBO object
     */
    FBO()
    {
        glGenFramebuffers(1, &id);
    }

    /**
     * Deletes FBO
     */
    ~FBO()
    {
        glDeleteFramebuffers(1, &id);
    }

    /**
     * @return FBO id
     */
    [[nodiscard]] inline unsigned int Get() const { return id; }

    /**
     * Attaches texture to the FBO
     * @param texture texture id
     * @param attachmentType attachment type
     */
    void AddTexture(unsigned int texture, unsigned int attachmentType)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        bufferTextures.push_back(texture);
    }

    void AddTexture(unsigned int texture, unsigned int textureType, unsigned int attachmentType)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textureType, texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        bufferTextures.push_back(texture);
    }

    /**
     * Sets OpenGL glDrawBuffer to given value
     * @param mode glDrawBuffer mode
     */
    inline void SetDrawBuffer(unsigned int mode) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glDrawBuffer(mode);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /**
     * Sets OpenGL glReadBuffer to given value
     * @param mode glReadBuffer mode
     */
    inline void SetReadBuffer(unsigned int mode) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glReadBuffer(mode);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /**
     * Binds this FBO
     */
    inline void Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }

    /**
     * Binds current FBO to 0
     */
    static inline void Reset()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

private:
    unsigned int id { 0 };
    std::vector<unsigned int> bufferTextures;
};

#endif //GRAPHICS_FBO_HPP
