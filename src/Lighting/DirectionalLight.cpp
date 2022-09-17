//
// Created by Anton on 11.08.2022.
//

#include "DirectionalLight.h"
#include "../Logging/easylogging++.h"

/**
 *
 * @param dir
 * @param amb
 * @param diff
 * @param spec
 */
DirectionalLight::DirectionalLight(const glm::vec3& dir, const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec) : direction(dir), ambient(amb), diffuse(diff), specular(spec){}
