//
// Created by Anton on 12.12.2021.
//

#ifndef GRAPHICS_PERSPECTIVECAMERA_H
#define GRAPHICS_PERSPECTIVECAMERA_H

#include "glm/glm.hpp"
#include <iostream>

enum CameraProjection : unsigned int
{
    Ortho = 0,
    Perspective = 1
};

class Camera
{
public:
    /**
     * Camera constructor
     * @param fov camera field of view, in radians
     */
    explicit Camera(float fov);

    /**
     * Camera constructor
     * @param fov camera field of view, in radians
     * @param aspectRatio camera aspect ratio
     */
    explicit Camera(float fov, const glm::vec2& aspectRatio);

    /**
     * Updates camera internal matrices and vectors, must be called before getting view and projection matrices
     * @param rotation camera rotation
     */
    void Update(const glm::vec3& rotation);

    /**
     * Set new field of view
     * @param newFOV new field of view, in radians
     */
    inline void SetFieldOfView(float newFOV)
    {
        fov = newFOV;
    };

    /**
     * Sets new ortho width, works only in orthogonal projection mode
     * @param newWidth new ortho width
     */
    inline void SetOrthoWidth(float newWidth)
    {
        orthoWidth = newWidth;
    }

    /**
     * Sets new ortho height, works only in orthogonal projection mode
     * @param newHeight new ortho height
     */
    inline void SetOrthoHeight(float newHeight)
    {
        orthoHeight = newHeight;
    }

    /**
     * @return current orthogonal width
     */
    [[nodiscard]] inline float GetOrthoWidth() const
    {
        return orthoWidth;
    }

    /**
     * @return current orthogonal height
     */
    [[nodiscard]] inline float GetOrthoHeight() const
    {
        return orthoHeight;
    }

    /**
     * Zooms camera without changing its FOV
     * @param newZoom new zoom value, 1.0 default
     */
    inline void ZoomTo(float newZoom = 1.0f)
    {
        zoom = newZoom;
    }

    /**
     * Changes zoom by deltaZoom value, without touching camera FOV
     * @param deltaZoom value to add to current zoom value
     */
    inline void Zoom(float deltaZoom = 0.0f)
    {
        zoom += deltaZoom;
    }

    /**
     * Get camera projection matrix
     * @return projection matrix
     */
    [[nodiscard]] glm::mat4 GetProjection() const;

    [[nodiscard]] glm::mat4 GetInfiniteProjection() const;


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

    /**
     * @return camera direction vector
     */
    [[nodiscard]] inline const glm::vec3& GetDirection() const { return dir; };

    /**
    * @return camera selected aspect ratio as vector
    */
    [[nodiscard]] inline float GetAspectRatioFloat() const { return aspectRatio.x / aspectRatio.y; }

public:
    glm::vec2 aspectRatio;
    CameraProjection projection = CameraProjection::Perspective;

private:
    float fov, zoom;
    float nearPlane = 0.005F, farPlane = 2097152.0F;
    float orthoWidth = 20.0F, orthoHeight = 20.0F;

    glm::vec3 front, right, dir, up;
    glm::mat4 model;
};

#endif //GRAPHICS_PERSPECTIVECAMERA_H
