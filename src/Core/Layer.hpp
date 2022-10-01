//
// Created by Anton on 01.10.2022.
//

#ifndef GRAPHICS_LAYER_HPP
#define GRAPHICS_LAYER_HPP

#include <memory>
#include "../Entity/Scene.h"

class Layer
{
public:
    virtual void OnCreate();
    virtual void OnDestroy();

    virtual void OnUiRender();
    virtual void OnUpdate(double deltaTime);

    virtual ~Layer();
};


#endif //GRAPHICS_LAYER_HPP
