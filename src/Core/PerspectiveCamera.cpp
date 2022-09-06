//
// Created by Anton on 12.12.2021.
//

#include "Window.h"
#include "Global.h"
#include "../include/OpenGL/include/glm/ext.hpp"
#include "PerspectiveCamera.h"


PerspectiveCamera::PerspectiveCamera(const glm::vec3& position, float fov) : fov(fov), zoom(1.0F), posX(0.0), posY(0.0)
{
    this->position = position;
    model = glm::mat4(1.0F);
    front = glm::vec3(model * glm::vec4(0,0,-1,1));
    right = glm::vec3(model * glm::vec4(1,0,0,1));
    up    = glm::vec3(model * glm::vec4(0,1,0,1));
    dir   = glm::vec3(model * glm::vec4(0,0,-1,1));

    dir.y = 0;

    float len = glm::length(dir);

    if (len > 0.0F)
    {
        dir.x /= len;
        dir.z /= len;
    }
}

glm::mat4 PerspectiveCamera::GetProjection() const
{
    float aspect = ((float) Window::GetWidth()) / (float) Window::GetHeight();
    return glm::perspective(fov * zoom, aspect, nearPlane, farPlane);
}

glm::mat4 PerspectiveCamera::GetView() const
{
    return glm::lookAt(position, position + front, up);
}


void PerspectiveCamera::UpdateControls()
{
    double delta = Global::GetWorldDeltaTime();
    int speed;

    /* PerspectiveCamera and meshes speed settings */
    if(EventsHandler::IsPressed(GLFW_KEY_LEFT_SHIFT))
    {
        speed = 10;
    }
    else if (EventsHandler::IsPressed(GLFW_KEY_LEFT_CONTROL))
    {
        speed = 1;
    }
    else
    {
        speed = 5;
    }

    /* PerspectiveCamera world position */
    if (EventsHandler::IsPressed(GLFW_KEY_W))
    {
        position += front * delta * speed;
    }
    if (EventsHandler::IsPressed(GLFW_KEY_S))
    {
        position += - front * delta * speed;
    }
    if (EventsHandler::IsPressed(GLFW_KEY_D))
    {
        position += right * delta * speed;
    }
    if (EventsHandler::IsPressed(GLFW_KEY_A))
    {
        position += - right * delta * speed;
    }

    float mouseSensitivity = 150.0F;
    /* PerspectiveCamera world orientation */
    if (EventsHandler::_cursor_locked)
    {
        posX += -EventsHandler::deltaX * delta * mouseSensitivity / (float) Window::GetHeight() * 2;
        posY += -EventsHandler::deltaY * delta * mouseSensitivity / (float) Window::GetHeight() * 2;

        if (posY < - glm::radians(89.0F))
        {
            posY = - glm::radians(89.0F);
        }
        if (posY > glm::radians(89.0F))
        {
            posY = glm::radians(89.0F);
        }

        rotation = glm::vec3(posY, posX, 0.0F);
        Update();
    }
}

void PerspectiveCamera::SetFieldOfView(float newFOV)
{
    fov = newFOV;
}

float PerspectiveCamera::GetFieldOfView() const
{
    return fov;
}

void PerspectiveCamera::SetDirection(const glm::vec3 &direction)
{
    dir = direction;
}

void PerspectiveCamera::Update()
{
    model = glm::mat4(1.0f);
    /* Yaw axis rotation */
    model = glm::rotate(model, rotation.z, glm::vec3(0,0,1));
    /* Pitch axis rotation */
    model = glm::rotate(model, rotation.y, glm::vec3(0,1,0));
    /* Roll axis rotation */
    model = glm::rotate(model, rotation.x, glm::vec3(1,0,0));

    front = glm::vec3(model * glm::vec4(0,0,-1,1));
    right = glm::vec3(model * glm::vec4(1,0,0,1));
    up = glm::vec3(model * glm::vec4(0,1,0,1));
    dir = glm::vec3(model * glm::vec4(0,0,-1,1));

    dir.y = 0;

    float len = glm::length(dir);

    if (len > 0.0f)
    {
        dir.x /= len;
        dir.z /= len;
    }
}