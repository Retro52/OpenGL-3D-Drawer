//
// Created by Anton on 30.08.2022.
//

#include "ShadowsHandler.h"
#include "../Core/InGameException.h"
#include "../Render/Renderer.h"


GLuint ShadowsHandler::shadowTexture, ShadowsHandler::shadowFBO = 0;

//void ShadowsHandler::Initialize()
//{
//}

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
    glViewport(0, 0, shadowMapResolution, shadowMapResolution);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
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

    return shadowTexture;
}
