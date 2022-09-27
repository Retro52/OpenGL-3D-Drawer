//
// Created by Anton on 11.08.2022.
//

#ifndef GRAPHICS_DIRECTIONALLIGHT_H
#define GRAPHICS_DIRECTIONALLIGHT_H

#include "../vendors/include/glm/glm.hpp"
#include "../vendors/include/glm/gtc/matrix_transform.hpp"

/* TODO: turn it into the singleton */
class DirectionalLight
{
public:
    /**
     * Creates scene directional light
     * @param dir directional strength
     * @param amb ambient strength
     * @param diff diffuse strength
     * @param spec specular strength
     */
    DirectionalLight(const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec);
    ~DirectionalLight() = default;

    /**
     * Returns dirlight direction based on entity rotation vector
     * @param rotation
     * @return
     */
    static glm::vec3 GetDirection(const glm::vec3& rotation)
    {
        glm::mat4 model = glm::mat4(1.0f);
        /* Yaw axis rotation */
        model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
        /* Pitch axis rotation */
        model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
        /* Roll axis rotation */
        model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));

        return glm::normalize(glm::vec3(model * glm::vec4(0, 0, -1, 1)));
    }

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};


#endif //GRAPHICS_DIRECTIONALLIGHT_H
