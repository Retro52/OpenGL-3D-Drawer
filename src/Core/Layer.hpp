//
// Created by Anton on 01.10.2022.
//

#ifndef GRAPHICS_LAYER_HPP
#define GRAPHICS_LAYER_HPP

#include <memory>
#include "Event.hpp"
#include "../Entity/Scene.h"

class Layer
{
public:
    virtual void OnCreate() = 0;
    virtual void OnDestroy() = 0;

    virtual void OnUiRender() = 0;
    virtual void OnUpdate(double deltaTime) = 0;

    // shader pointer is used for pointer arithmetics
    virtual void OnEvent(const std::shared_ptr<Event>& event) = 0;

    virtual ~Layer() = default;
};


#endif //GRAPHICS_LAYER_HPP
