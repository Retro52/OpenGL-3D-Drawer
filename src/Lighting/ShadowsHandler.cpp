//
// Created by Anton on 30.08.2022.
//

#include "ShadowsHandler.h"
#include "../Core/InGameException.h"
#include "../Core/ResourcesManager.h"
#include "../Core/Window.h"

GLuint ShadowsHandler::depthTexture, ShadowsHandler::framebufferName = 0;

void ShadowsHandler::Initialize()
{
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &framebufferName);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0,GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferName);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw InGameException("Shadows handler framebuffer is not OK");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint ShadowsHandler::RenderShadowMap()
{
    // Depth testing needed for Shadow Map
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Preparations for the Shadow Map
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferName);
    glClear(GL_DEPTH_BUFFER_BIT);
    for (auto & m : ResourcesManager::GetPlayerScene()->GetActors())
    {
        m->Draw(* ResourcesManager::GetShader("shadowShader"), 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::GetWidth(), Window::GetHeight());

    glDisable(GL_CULL_FACE);

    return depthTexture;
}
