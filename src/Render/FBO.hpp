//
// Created by Anton on 15.09.2022.
//

#ifndef GRAPHICS_FBO_HPP
#define GRAPHICS_FBO_HPP

#define GLEW_STATIC
#include "glew.h"
#include "glm/glm.hpp"
#include "Material.h"
#include <iostream>
#include <unordered_map>
#include "../Core/InGameException.h"

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
    void AddTexture(const std::shared_ptr<Texture>& texture, unsigned int attachmentType)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, texture->GetId(), 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        bufferTextures[attachmentType] = texture;
    }

    void AddTexture(const std::shared_ptr<Texture>& texture, unsigned int textureType, unsigned int attachmentType, bool isColorTexture = false)
    {
        if (isColorTexture)
        {
            colorTextureType = textureType;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textureType, texture->GetId(), 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        bufferTextures[attachmentType] = texture;
    }

    /**
     * Sets OpenGL glDrawBuffers to given value
     * @param count number of buffers
     * @param attachments buffers
     */
    inline void SetDrawBuffer(int count, unsigned int * attachments) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glDrawBuffers(count, attachments);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    std::shared_ptr<Texture>& GetColorTexture()
    {
        return bufferTextures.at(colorTextureType);
    }

    std::shared_ptr<Texture>& GetDepthStencilTexture()
    {
        return bufferTextures.at(GL_DEPTH_STENCIL_ATTACHMENT);
    }

    std::shared_ptr<Texture>& GetTexture(unsigned int attachmentType)
    {
        return bufferTextures.at(attachmentType);
    }

    void GenerateRenderBufferDepthAttachment(int width, int height)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glGenRenderbuffers(1, &rboId);
        glBindRenderbuffer(GL_RENDERBUFFER, rboId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Check() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        GAME_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "WINDOW::ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

private:
    unsigned int id { 0 };
    unsigned int rboId { 0 };
    unsigned int colorTextureType = GL_COLOR_ATTACHMENT0;
    std::unordered_map<unsigned int, std::shared_ptr<Texture>> bufferTextures;
};

#endif //GRAPHICS_FBO_HPP
