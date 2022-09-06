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
    static void Prepare(const PerspectiveCamera& camera)
    {
        Shader mShader = * ResourcesManager::GetShader("mainShader");
        mShader.Use();
        mShader.setInt("drawMode", 1);
        mShader.setMat4("view", camera.GetView());
        mShader.setVec3("ProjPos", camera.GetPosition());
        mShader.setMat4("projection", camera.GetProjection());
        mShader.setMat4("lightProjection", lightProjection);
        mShader.setPointLights(pLights);
        mShader.setDirLight(dLight);
    }
    static void Render(const TransformComponent& t, const Model3DComponent& m)
    {
        m.model.Draw(* ResourcesManager::GetShader("mShader"), t.GetTransform());
    }

    static void RenderToDepthBuffer(const TransformComponent& t, const Model3DComponent& m)
    {
        m.model.DrawIntoDepth(* ResourcesManager::GetShader("shadowShader"), t.GetTransform());
    }
};


#endif //GRAPHICS_RENDERER_H
