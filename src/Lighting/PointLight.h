//
// Created by Anton on 11.08.2022.
//

#ifndef GRAPHICS_POINTLIGHT_H
#define GRAPHICS_POINTLIGHT_H

#include "glm/glm.hpp"

class PointLight
{
public:
    /**
     * Creates point light
     * @param amb ambient strength
     * @param diff diffuse strength
     * @param spec specular strength
     * @param con constant
     * @param lin linear
     * @param quad quadratic
     */
    PointLight(const glm::vec3 &amb, const glm::vec3 &diff, const glm::vec3 &spec, float con, float lin, float quad);
    ~PointLight() = default;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

};


#endif //GRAPHICS_POINTLIGHT_H
