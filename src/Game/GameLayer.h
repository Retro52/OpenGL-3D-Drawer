//
// Created by Anton on 09.10.2022.
//

#ifndef GRAPHICS_GAMELAYER_H
#define GRAPHICS_GAMELAYER_H

#include "../Core/Layer.hpp"

class GameLayer : public Layer
{
public:
    virtual void OnCreate() override;
    virtual void OnDestroy() override;

    virtual void OnUiRender() override;
    virtual void OnUpdate(double deltaTime) override;

    // shader pointer is used for pointer arithmetics
    virtual void OnEvent(const std::shared_ptr<Event>& event) override;

    virtual ~GameLayer() override = default;
};


#endif //GRAPHICS_GAMELAYER_H
