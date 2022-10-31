//
// Created by Anton on 06.09.2022.
//

#include "UBO.hpp"
#include "Renderer.h"
#include "RendererIniSerializer.hpp"
#include "../Core/Profiler.hpp"


std::unique_ptr<UBO<glm::mat4x4, 16>> Renderer::lightMatricesUBO;
std::vector<float> Renderer::cascadeLevels({ 25.0f, 50.0f, 100.0f, 200.0f, 750.0f });

constexpr float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

void Renderer::Initialize()
{
    lightMatricesUBO = std::make_unique<UBO<glm::mat4x4, 16>>();

    viewportFBO    = std::make_unique<FBO>();
    postProcessFBO = std::make_unique<FBO>();

    RendererIniSerializer::LoadRendererSettings();

    cascadesCount = static_cast<int>(cascadeLevels.size());
    viewportFBO->AddTexture(std::make_shared<Texture>(
            fboWidth,
            fboHeight,
            GL_RGB,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            false
    ), GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0);


    viewportFBO->GenerateRenderBufferDepthAttachment(static_cast<int>(fboWidth), static_cast<int>(fboHeight));

    postProcessFBO->AddTexture(std::make_shared<Texture>(
            fboWidth,
            fboHeight,
            GL_RGB,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            false
    ), GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0);

    viewportFBO->Check();
    postProcessFBO->Check();

    FBO::Reset();

    shadowTexture = std::make_shared<Texture>(
            shadowMapResolution,
            shadowMapResolution,
            GL_DEPTH_COMPONENT,
            GL_DEPTH_COMPONENT32F,
            GL_UNSIGNED_BYTE,
            false,
            true,
            cascadesCount
    );

    shadowFBO = std::make_unique<FBO>();

    shadowFBO->AddTexture(shadowTexture, GL_DEPTH_ATTACHMENT);
    shadowFBO->SetDrawBuffer(GL_NONE);
    shadowFBO->SetReadBuffer(GL_NONE);
    shadowFBO->Check();
    shadowFBO->Reset();

    gBufferFBO = std::make_unique<FBO>();
    lBufferFBO = std::make_unique<FBO>();

    gBufferFBO->AddTexture(
            std::make_shared<Texture>
                    (
                            fboWidth,
                            fboHeight,
                            GL_RGBA,
                            GL_RGBA32F,
                            GL_FLOAT
                            ),
            GL_COLOR_ATTACHMENT0
            );

    gBufferFBO->AddTexture(
            std::make_shared<Texture>
                    (
                            fboWidth,
                            fboHeight,
                            GL_RGBA,
                            GL_RGBA32F,
                            GL_FLOAT
                    ),
            GL_COLOR_ATTACHMENT1
    );

    gBufferFBO->AddTexture(
            std::make_shared<Texture>
                    (
                            fboWidth,
                            fboHeight,
                            GL_RGBA,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE
                    ),
            GL_COLOR_ATTACHMENT2
    );

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    gBufferFBO->SetDrawBuffer(3, attachments);
    gBufferFBO->GenerateRenderBufferDepthAttachment(static_cast<int>(fboWidth), static_cast<int>(fboHeight));
    gBufferFBO->Check();
    FBO::Reset();

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

}

void Renderer::ShutDown()
{
    RendererIniSerializer::SerializeRendererSettings();
}

void Renderer::Prepare(Scene &scene)
{
    Clear(glm::vec3(0, 0, 0));

    std::shared_ptr<Shader>& gShader = ResourcesManager::GetShader("gBufferShader");
    std::shared_ptr<Shader>& lShader = ResourcesManager::GetShader("lBufferShader");

    auto primaryCamera = scene.GetPrimaryCamera();

    if(!primaryCamera)
    {
        return;
    }
    auto& cameraComponent = primaryCamera->GetComponent<CameraComponent>();
    auto& cameraTransform = primaryCamera->GetComponent<TransformComponent>();
    const auto& camera = cameraComponent.camera;

    cameraComponent.UpdateCamera(cameraTransform.rotation);
    const auto cameraView = cameraComponent.GetCameraView(cameraTransform.translation);

    gShader->Use();
    gShader->setMat4("view", cameraView);
    gShader->setMat4("projection", cameraComponent.GetCameraProjection());

    lShader->Use();
    lShader->setInt("drawMode", drawMode);
    lShader->setMat4("view", cameraView);
    lShader->setVec3("ProjPos", cameraTransform.translation);

    auto sceneDirLight = scene.GetDirectionalLight();

    if(sceneDirLight)
    {
        const auto& dLight = sceneDirLight->GetComponent<DirectionalLightComponent>().directionalLight;
        const auto& dlRotation = scene.GetDirectionalLight()->GetComponent<TransformComponent>().rotation;

        auto lightMatrices = getLightSpaceMatrices(camera.GetNearPlane(), camera.GetFarPlane(), camera.GetFieldOfView(), camera.GetAspectRatioFloat(), DirectionalLight::GetDirection(dlRotation), cameraView, cascadeLevels);

        lShader->setBool("dLight.isPresent", true);
        lShader->setDirLight(dLight, dlRotation);

        lShader->setFloat("farPlane", camera.GetFarPlane());
        lShader->setInt("cascadeCount", (int) cascadeLevels.size());

        for (size_t i = 0; i < cascadeLevels.size(); ++i)
        {
            lShader->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", cascadeLevels[i]);
        }

        lightMatricesUBO->Bind();
        lightMatricesUBO->FillData(lightMatrices);
        lightMatricesUBO->Reset();
    }

    int idx = -1;
    const auto& view = scene.registry.view<TransformComponent, PointLightComponent>();
    for (const auto& entity : view)
    {
        idx++;
        auto [t, p] = view.get<TransformComponent, PointLightComponent>(entity);
        lShader->setPointLight(idx, p.pointLight, t.translation);
    }
    lShader->setInt("pointLightsCount", idx + 1);
}

void Renderer::Render(Scene &scene)
{
    // enabling wireframe if necessary
    if(drawMode == 0)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    Profiler::StartGPass();
    GeometryPass(scene);
    Profiler::EndGPass();

    Profiler::StartSPass();
    RenderShadowMaps(scene);
    Profiler::EndSPass();

    Profiler::StartLPass();
    LightingPass(scene);
    Profiler::EndLPass();

//    glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
//
//    ResourcesManager::GetShader("postProcessShader")->Use();
//    viewportFBO->GetColorTexture()->Bind();
//    RenderQuad();
//    FBO::Reset();

//    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFBO->Get());
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
//    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
//    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
//    // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
//    glBlitFramebuffer(0, 0, fboWidth, fboHeight, 0, 0, fboWidth, fboHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
////    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    FBO::Reset();
}

void Renderer::GeometryPass(Scene &scene)
{
    const auto& view = scene.registry.view<TransformComponent, Model3DComponent>();
    auto& shader = ResourcesManager::GetShader("gBufferShader");

    gBufferFBO->Bind();
    Clear(glm::vec3(0, 0, 0));

    glViewport(0, 0, static_cast<int>(fboWidth), static_cast<int>(fboHeight));

    shader->Use();
    for (const auto& entity : view)
    {
        auto [t, m] = view.get<TransformComponent, Model3DComponent>(entity);

        shader->setInt("material.tilingFactor", m.tilingFactor);
        m.model.Draw(shader, t.GetTransform());
    }
}

void Renderer::LightingPass(Scene &scene)
{
    if(!shouldDrawFinalToFBO && !isPostProcessingActivated)
    {
        FBO::Reset();
        glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
    }
    else
    {
        viewportFBO->Bind();
        glViewport(0, 0, static_cast<int>(fboWidth), static_cast<int>(fboHeight));
    }

    Clear(glm::vec3(0, 0, 0));

    auto& lShader = ResourcesManager::GetShader("lBufferShader");

    lShader->Use();
    lShader->setInt("gNormal", 1);
    lShader->setInt("gPosition", 0);
    lShader->setInt("gAlbedoSpec", 2);
    lShader->setInt("dLight.mapShadow", 3);

    glActiveTexture(GL_TEXTURE0);
    gBufferFBO->GetTexture(GL_COLOR_ATTACHMENT0)->Bind();

    glActiveTexture(GL_TEXTURE1);
    gBufferFBO->GetTexture(GL_COLOR_ATTACHMENT1)->Bind();

    glActiveTexture(GL_TEXTURE2);
    gBufferFBO->GetTexture(GL_COLOR_ATTACHMENT2)->Bind();

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowTexture->GetId());
    glActiveTexture(GL_TEXTURE0);
    RenderQuad();

    FBO::Reset();
}

void Renderer::RenderShadowMaps(Scene &scene)
{
    // Depth testing needed for Shadow Map
    EnableDepthTesting();

    //disabling face culling
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);

    // adjusting bias by shadow map resolution
    float offsetScale = (static_cast<float>(shadowMapResolution) / 2048) - 1.0f;

    // calculating bias
    float slopeOffset = baseOffset + (deltaOffset * offsetScale);

    glPolygonOffset(slopeOffset, slopeOffset * factorMultiplier);

    // Creating separate viewport for shadow map
    glViewport(0, 0, shadowMapResolution, shadowMapResolution);

    shadowFBO->Bind();

    glClear(GL_DEPTH_BUFFER_BIT);
    RenderToDepthBuffer(scene);

    shadowFBO->Reset();

    // re-enabling cull faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDisable(GL_POLYGON_OFFSET_FILL);
}

void Renderer::RenderToDepthBuffer(Scene &scene)
{
    auto& shader = ResourcesManager::GetShader("shadowShader");
    const auto& view = scene.registry.view<TransformComponent, Model3DComponent>();
    shader->Use();
    for (const auto& entity : view)
    {
        auto [t, m] = view.get<TransformComponent, Model3DComponent>(entity);
        if(m.castsShadow)
        {
            m.model.DrawIntoDepth(* shader, t.GetTransform());
        }
    }
}

std::vector<glm::mat4>
Renderer::getLightSpaceMatrices(float cameraNearPlane, float cameraFarPlane, float zoom, float aspectRatio, const glm::vec3 &lightDir, const glm::mat4 &viewMatrix, std::vector<float> &shadowCascadeLevels)
{
    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(cameraNearPlane, shadowCascadeLevels[i], zoom, aspectRatio, lightDir, viewMatrix));
        }
        else if (i < shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i], zoom, aspectRatio, lightDir, viewMatrix));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], cameraFarPlane, zoom, aspectRatio, lightDir, viewMatrix));
        }
    }
    return ret;
}

glm::mat4 Renderer::getLightSpaceMatrix(float nearPlane, float farPlane, float zoom, float aspectRatio, const glm::vec3 &lightDir, const glm::mat4 &viewMatrix)
{
    const auto proj = glm::perspective(zoom, aspectRatio, nearPlane, farPlane);
    const auto corners = getFrustumCornersWorldSpace(proj, viewMatrix);

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center - lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProjection * lightView;
}

std::vector<glm::vec4> Renderer::getFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view)
{
    return getFrustumCornersWorldSpace(proj * view);
}

std::vector<glm::vec4> Renderer::getFrustumCornersWorldSpace(const glm::mat4 &projview)
{
    const auto inv = glm::inverse(projview);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * static_cast<float>(x) - 1.0f, 2.0f * static_cast<float>(y) - 1.0f, 2.0f * static_cast<float>(z) - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}
