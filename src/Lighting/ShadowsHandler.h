//
// Created by Anton on 30.08.2022.
//

#ifndef GRAPHICS_SHADOWSHANDLER_H
#define GRAPHICS_SHADOWSHANDLER_H

#define GLEW_STATIC

#include "../vendors/include/GLEW/glew.h"
#include "../vendors/include/glm/glm.hpp"
#include "../vendors/include/glm/gtc/matrix_transform.hpp"
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
    inline static GLuint GetRenderedShadowMap() { return shadowTexture; };

private:
    static GLuint shadowTexture;

    static const int shadowMultiSampler = 2;
    static const int shadowMapResolution = 1024 * shadowMultiSampler;

    static std::unique_ptr<FBO> shadowFBO;
};


#endif //GRAPHICS_SHADOWSHANDLER_H
