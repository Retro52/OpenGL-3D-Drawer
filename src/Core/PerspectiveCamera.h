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
     * @param fov camera field of view, in radians
     */
    explicit PerspectiveCamera(float fov);

    /**
     * Updates camera internal matrices and vectors, must be called before getting view and projection matrices
     * @param rotation camera rotation
     */
    void Update(const glm::vec3& rotation);

    /**
     * Set new field of view
     * @param newFOV new field of view, in radians
     */
    inline void SetFieldOfView(float newFOV) { fov = newFOV; };

    /**
     * Get camera projection matrix
     * @return projection matrix
     */
    [[nodiscard]] glm::mat4 GetProjection() const;

    /**
     * Get camera view matrix
     * @param position camera position
     * @param rotation camera rotation
     * @return view matrix
     */
    [[nodiscard]] glm::mat4 GetView(const glm::vec3& position);

    /**
     * @return field of view, in radians
     */
    [[nodiscard]] inline float GetFieldOfView() const { return fov; };

    /**
     * @return far plane value
     */
    [[nodiscard]] inline float GetFarPlane()  const { return farPlane; }

    /**
     * @return near plane value
     */
    [[nodiscard]] inline float GetNearPlane() const { return nearPlane; }

    /**
     * @return up vector
     */
    [[nodiscard]] inline const glm::vec3& GetUpVector() const { return up; };

    /**
    * @return front vector
    */
    [[nodiscard]] inline const glm::vec3& GetFrontVector() const { return front; };

    /**
     * @return right vector
     */
    [[nodiscard]] inline const glm::vec3& GetRightVector() const { return right; };
public:
    double posX, posY;

private:
    float fov, zoom;
    const float nearPlane = 0.05f, farPlane = 1500.0f;
    glm::vec3 front, right, dir, up;
    glm::mat4 model;
};

#endif //GRAPHICS_PERSPECTIVECAMERA_H
