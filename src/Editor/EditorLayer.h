//
// Created by Anton on 01.10.2022.
//

#ifndef GRAPHICS_EDITORLAYER_H
#define GRAPHICS_EDITORLAYER_H

#include "../Core/Layer.hpp"

class EditorLayer : public Layer
{
    virtual void OnCreate() override;

    virtual void OnDestroy() override;

    virtual void OnUiRender() override;

    virtual void OnUpdate(double deltaTime) override;

    virtual ~EditorLayer();
};


#endif //GRAPHICS_EDITORLAYER_H
