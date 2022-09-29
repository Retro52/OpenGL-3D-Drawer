//
// Created by Anton on 30.08.2022.
//

#ifndef GRAPHICS_SHADOWSHANDLER_H
#define GRAPHICS_SHADOWSHANDLER_H

#define GLEW_STATIC

#include "glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../Core/ResourcesManager.h"
#include "../Core/Window.h"
#include "../Render/FBO.hpp"


class ShadowsHandler
{
public:
    /**
     * Initializes framebuffer and shadow map texture array
     * @param size number of shadow maps in texture array
     */
    static void Initialize(int size);

    /**
     * Renders directional light shadow map
     * @return texture id
     */
    static GLuint RenderShadowMap();

    /**
     * @return last rendered shadow map texture id
     */
    static GLuint GetRenderedShadowMap() { return shadowTexture; };

    /**
     * Sets shadowMapResolution. Has to be called BEFORE Initialization
     * @param resolution shadow map resolution
     */
    static void SetMapResolution(int resolution) { shadowMapResolution = resolution; };
private:
    static GLuint shadowTexture;

    static int shadowMapResolution;

    static std::unique_ptr<FBO> shadowFBO;
};

inline int ShadowsHandler::shadowMapResolution = 1024;

#endif //GRAPHICS_SHADOWSHANDLER_H
