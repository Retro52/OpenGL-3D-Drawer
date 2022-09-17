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
#include "../Render/FBO.hpp"


class ShadowsHandler
{
public:
    static void Initialize(int size);

    static GLuint RenderShadowMap();

    inline static GLuint GetRenderedShadowMap() { return shadowTexture; };

private:
    static GLuint shadowTexture;

    static const int shadowMultiSampler = 3;
    static const int shadowMapResolution = 1024 * shadowMultiSampler;

    static std::unique_ptr<FBO> shadowFBO;
};


#endif //GRAPHICS_SHADOWSHANDLER_H
