//
// Created by Anton on 06.09.2022.
//

#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#define GLEW_STATIC

#include "../include/OpenGL/include/GLEW/glew.h"
#include "../Core/ResourcesManager.h"
#include "../Entity/Components.h"
#include "../Entity/Entity.h"


class Renderer
{
public:
    static void Prepare(const Entity& camera)
    {
        Shader mShader = * ResourcesManager::GetShader("mainShader");
        PerspectiveCamera pCamera = camera.GetComponent<CameraComponent>().camera;
        TransformComponent cameraTransform = camera.GetComponent<TransformComponent>();

        // Temporary
        glm::mat4 lightOrtho = glm::ortho(-35.0f, 35.0f, 35.0f, -35.0f, 0.1f, 75.0f);
        glm::mat4 lightView = glm::lookAt(20.f * (- glm::vec3(-1.0, -0.2, -0.2)), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightProjection = lightOrtho * lightView;

        mShader.Use();
        mShader.setInt("drawMode", 1);
        mShader.setMat4("view", pCamera.GetView());
        mShader.setVec3("ProjPos", cameraTransform.translation);
        mShader.setMat4("projection", pCamera.GetProjection());
        mShader.setMat4("lightProjection", lightProjection);

        glClearColor(0.203f, 0.76f, 0.938f,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        mShader.setPointLights(pLights);
//        mShader.setDirLight(dLight);
    }
    static void Render(const TransformComponent& t, const Model3DComponent& m)
    {
        m.model.Draw(* ResourcesManager::GetShader("mainShader"), t.GetTransform());
    }

    static void RenderToDepthBuffer(const TransformComponent& t, const Model3DComponent& m)
    {
        m.model.DrawIntoDepth(* ResourcesManager::GetShader("shadowShader"), t.GetTransform());
    }
};


#endif //GRAPHICS_RENDERER_H
