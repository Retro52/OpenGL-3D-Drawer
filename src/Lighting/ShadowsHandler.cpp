//
// Created by Anton on 30.08.2022.
//

#include "ShadowsHandler.h"
#include "../Core/InGameException.h"
#include "../Render/Renderer.h"


std::unique_ptr<FBO> ShadowsHandler::shadowFBO = nullptr;
std::shared_ptr<Texture> ShadowsHandler::shadowTexture = nullptr;


GLuint ShadowsHandler::RenderShadowMap()
{
    // Depth testing needed for Shadow Map
    Renderer::EnableDepthTesting();

    //disabling face culling
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);

    // constants
    constexpr float baseOffset        = 8.96f;
    constexpr float deltaOffset       = 2.96f;
    constexpr float factorMultiplier  = 1.44f;

    // adjusting bias by shadow map resolution
    const float offsetScale = (static_cast<float>(shadowMapResolution) / 2048) - 0.5f;
    // calculating bias
    const float slopeOffset = baseOffset + (deltaOffset * offsetScale);

    glPolygonOffset(slopeOffset, slopeOffset * factorMultiplier);

    // Creating separate viewport for shadow map
    glViewport(0, 0, shadowMapResolution, shadowMapResolution);

    shadowFBO->Bind();

    glClear(GL_DEPTH_BUFFER_BIT);
    Renderer::RenderToDepthBuffer(* ResourcesManager::GetPlayerScene());

    shadowFBO->Reset();

    glViewport(0, 0, (int) Renderer::GetFboWidth(), (int) Renderer::GetFboHeight());

    // re-enabling cull faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDisable(GL_POLYGON_OFFSET_FILL);

    return shadowTexture->GetId();
}

void ShadowsHandler::Initialize(const int size)
{
    shadowTexture = std::make_shared<Texture>(
            shadowMapResolution,
            shadowMapResolution,
            GL_DEPTH_COMPONENT,
            GL_DEPTH_COMPONENT32F,
            GL_UNSIGNED_BYTE,
            false,
            true,
            size
            );

    shadowFBO = std::make_unique<FBO>();

    shadowFBO->AddTexture(shadowTexture, GL_DEPTH_ATTACHMENT);
    shadowFBO->SetDrawBuffer(GL_NONE);
    shadowFBO->SetReadBuffer(GL_NONE);
    shadowFBO->Check();
    shadowFBO->Reset();
}
