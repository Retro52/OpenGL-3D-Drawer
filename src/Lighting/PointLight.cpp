//
// Created by Anton on 11.08.2022.
//

#include "PointLight.h"
#include "../Logging/easylogging++.h"

/**
 * Point light constructor
 * @param con
 * @param lin
 * @param quad
 * @param amb
 * @param diff
 * @param spec
 */
PointLight::PointLight(const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec, float con, float lin, float quad) :
        constant(con), linear(lin), quadratic(quad), ambient(amb), diffuse(diff), specular(spec) {}
