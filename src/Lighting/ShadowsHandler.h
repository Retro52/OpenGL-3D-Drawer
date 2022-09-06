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
    static void Initialize();
    static GLuint RenderShadowMap();

    inline static GLuint GetRenderedShadowMap() { return depthTexture; };

private:
    static GLuint depthTexture, framebufferName;

    static const int shadowMultiSampler = 1;
    static const int shadowMapWidth = 1024 * shadowMultiSampler, shadowMapHeight = 1024 * shadowMultiSampler;

    constexpr static const float shadowDistance = 500.0F;
};


#endif //GRAPHICS_SHADOWSHANDLER_H
