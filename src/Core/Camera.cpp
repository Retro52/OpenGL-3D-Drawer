//
// Created by Anton on 12.12.2021.
//

#include "Window.h"
#include "MainLoop.h"
#include <glm/ext.hpp>
#include "Camera.h"


Camera::Camera(float fov) : fov(fov), zoom(1.0F), aspectRatio(glm::vec2(16, 9))
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

Camera::Camera(float fov, const glm::vec2& aspectRatio) : fov(fov), zoom(1.0F), aspectRatio(aspectRatio)
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

void Camera::Update(const glm::vec3& rotation)
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

glm::mat4 Camera::GetProjection() const
{
    if (projection == CameraProjection::Perspective)
    {
        return glm::perspective(fov * zoom, aspectRatio.x / aspectRatio.y, nearPlane, farPlane);
    }
    else
    {
        return glm::ortho(orthoWidth / 2, orthoHeight / 2, -orthoHeight / 2, -orthoWidth / 2, nearPlane, farPlane);
    }
}

glm::mat4 Camera::GetInfiniteProjection() const
{
    if (projection == CameraProjection::Perspective)
    {
        return glm::infinitePerspective	(fov * zoom, aspectRatio.x / aspectRatio.y, nearPlane);
    }
    else
    {
        return glm::ortho(-orthoWidth / 2, orthoWidth / 2, -orthoHeight / 2, orthoHeight / 2, nearPlane, 2097152.0F);
    }
}

glm::mat4 Camera::GetView(const glm::vec3& position)
{
    return glm::lookAt(position, position + front, up);
}