//
// Created by Anton on 30.08.2022.
//

#ifndef GRAPHICS_SHADOWSHANDLER_H
#define GRAPHICS_SHADOWSHANDLER_H

#define GLEW_STATIC

#include "../include/OpenGL/include/GLEW/glew.h"
#include "../include/OpenGL/include/glm/glm.hpp"


class ShadowsHandler
{
public:
    static void Initialize();
    static GLuint RenderShadowMap();

private:
    static GLuint depthTexture, framebufferName;

    static const int shadowMultiSampler = 4;
    static const int shadowMapWidth = 1024 * shadowMultiSampler, shadowMapHeight = 1024 * shadowMultiSampler;
};


#endif //GRAPHICS_SHADOWSHANDLER_H
