//
// Created by Anton on 30.08.2022.
//

#ifndef GRAPHICS_SHADOWSHANDLER_H
#define GRAPHICS_SHADOWSHANDLER_H

#define GLEW_STATIC

#include "../include/OpenGL/include/GLEW/glew.h"
#include "../include/OpenGL/include/glm/glm.hpp"
#include "../include/OpenGL/include/glm/gtc/matrix_transform.hpp"
#include "../Core/ResourcesManager.h"
#include "../Core/Window.h"


class ShadowsHandler
{
public:
    static void Initialize(const int size)
    {
        glGenFramebuffers(1, &shadowFBO);

        glGenTextures(1, &shadowTexture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, shadowTexture);
        glTexImage3D(
                GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, shadowMapResolution, shadowMapResolution, size + 1,
                0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GAME_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
    static GLuint RenderShadowMap();

    inline static GLuint GetRenderedShadowMap() { return shadowTexture; };

private:
    static GLuint shadowTexture, shadowFBO;

    static const int shadowMultiSampler = 2;
    static const int shadowMapResolution = 1024 * shadowMultiSampler;
};


#endif //GRAPHICS_SHADOWSHANDLER_H
