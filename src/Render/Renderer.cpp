//
// Created by Anton on 06.09.2022.
//

#include "Renderer.h"
#include "UBO.hpp"

std::unique_ptr<UBO<glm::mat4x4, 16>> Renderer::lightMatricesUBO;
std::vector<float> Renderer::cascadeLevels({ 25.0f, 50.0f, 100.0f, 200.0f, 750.0f });