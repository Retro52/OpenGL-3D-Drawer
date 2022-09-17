//
// Created by Anton on 12.12.2021.
//

#include "Window.h"
#include "Global.h"
#include "../include/OpenGL/include/glm/ext.hpp"
#include "PerspectiveCamera.h"


PerspectiveCamera::PerspectiveCamera(float fov) : fov(fov), zoom(1.0F)
{
    model = glm::mat4(1.0F);
    front = glm::vec3(model * glm::vec4(0, 0, -1, 1));
    right = glm::vec3(model * glm::vec4(1, 0, 0, 1));
    up    = glm::vec3(model * glm::vec4(0, 1, 0, 1));
    dir   = glm::vec3(model * glm::vec4(0, 0, -1, 1));

    dir.y = 0;
    float len = glm::length(dir);

    if (len > 0.0F)
    {
        dir.x /= len;
        dir.z /= len;
    }
}

void PerspectiveCamera::Update(const glm::vec3& rotation)
{
    model = glm::mat4(1.0f);
    /* Yaw axis rotation */
    model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
    /* Pitch axis rotation */
    model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
    /* Roll axis rotation */
    model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));

    front = glm::vec3(model * glm::vec4(0, 0, -1, 1));
    right = glm::vec3(model * glm::vec4(1, 0, 0, 1));
    up = glm::vec3(model * glm::vec4(0, 1, 0, 1));
    dir = glm::vec3(model * glm::vec4(0, 0, -1, 1));

    dir.y = 0;

    float len = glm::length(dir);

    if (len > 0.0f)
    {
        dir.x /= len;
        dir.z /= len;
    }
}

glm::mat4 PerspectiveCamera::GetProjection() const
{
    return glm::perspective(fov * zoom, Window::GetAspectRatio(), nearPlane, farPlane);
}

glm::mat4 PerspectiveCamera::GetView(const glm::vec3& position)
{
    return glm::lookAt(position, position + front, up);
}