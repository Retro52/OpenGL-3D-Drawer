//
// Created by Anton on 30.08.2022.
//

#include "ShadowsHandler.h"
#include "../Core/InGameException.h"
#include "../Render/Renderer.h"


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

    float clampColor[] = { 1.0F, 1.0F, 1.0F, 1.0F };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (float *) clampColor);

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

    //disabling face culling
    glDisable(GL_CULL_FACE);

    // enabling polygon offset
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.1F, 4.0F);

    // Creating separate viewport for shadow map
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferName);
    glClear(GL_DEPTH_BUFFER_BIT);
    Renderer::Render(* ResourcesManager::GetPlayerScene());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::GetWidth(), Window::GetHeight());

    // re-enabling cull faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // disabling polygon offset
    glPolygonOffset(0, 0);
    glDisable(GL_POLYGON_OFFSET_FILL);

    return depthTexture;
}
