//
// Created by Anton on 12.12.2021.
//

#ifndef GRAPHICS_PERSPECTIVECAMERA_H
#define GRAPHICS_PERSPECTIVECAMERA_H

#include "../include/OpenGL/include/glm/glm.hpp"
#include <iostream>

class PerspectiveCamera
{
public:
    /**
     * Camera constructor
     * @param position camera position
     * @param fov camera field of view (60 - 100 range is recommended), in degrees
     */
    PerspectiveCamera(const glm::vec3& position, float fov);

    /**
     * Updates camera matrices every frame
     */
    void Update();

    /**
     * Updates camera based on the inputs
     */
    void UpdateControls();

    /**
     * Get camera projection matrix
     * @return projection matrix
     */
    glm::mat4 GetProjection() const;

    /**
     * Get camera view matrix
     * @return view matrix
     */
    glm::mat4 GetView() const;

    /**
     * Set new field of view
     * @param newFOV new field of view, in degrees
     */
    void SetFieldOfView(float newFOV);

    /**
     * Get camera field of view
     * @return vield of view, in degrees
     */
    float GetFieldOfView() const;

    void SetDirection(const glm::vec3& direction);

    float GetFarPlane()  const { return farPlane; }

    float GetNearPlane() const { return nearPlane; }
public:
    float fov, zoom;
    double posX, posY;
    const float nearPlane = 0.05f, farPlane = 1500.0f;
    glm::vec3 front, right, dir, up, position, rotation;
    glm::mat4 model;
};

#endif //GRAPHICS_PERSPECTIVECAMERA_H
