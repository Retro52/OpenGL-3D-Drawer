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
    static void Prepare(const Entity& camera);
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
