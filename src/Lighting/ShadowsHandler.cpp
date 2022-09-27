//
// Created by Anton on 30.08.2022.
//

#include "ShadowsHandler.h"
#include "../Core/InGameException.h"
#include "../Render/Renderer.h"


GLuint ShadowsHandler::shadowTexture;
std::unique_ptr<FBO> ShadowsHandler::shadowFBO = nullptr;


GLuint ShadowsHandler::RenderShadowMap()
{
    // Depth testing needed for Shadow Map
    glEnable(GL_DEPTH_TEST);

    //disabling face culling
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);

    glPolygonOffset(8.96f, 12.8f);

    // Creating separate viewport for shadow map
    glViewport(0, 0, shadowMapResolution, shadowMapResolution);

    shadowFBO->Bind();

    glClear(GL_DEPTH_BUFFER_BIT);
    Renderer::RenderToDepthBuffer(* ResourcesManager::GetPlayerScene());

    shadowFBO->Reset();

    glViewport(0, 0, Window::GetWidth(), Window::GetHeight());

    // re-enabling cull faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDisable(GL_POLYGON_OFFSET_FILL);

    return shadowTexture;
}

void ShadowsHandler::Initialize(const int size)
{

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

    shadowFBO = std::make_unique<FBO>();

    shadowFBO->AddTexture(shadowTexture, GL_DEPTH_ATTACHMENT);
    shadowFBO->SetDrawBuffer(GL_NONE);
    shadowFBO->SetReadBuffer(GL_NONE);

    GAME_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
}
