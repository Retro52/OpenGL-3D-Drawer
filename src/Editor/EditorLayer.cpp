//
// Created by Anton on 01.10.2022.
//

#include "EditorLayer.h"

void EditorLayer::OnCreate()
{
    std::cerr << "Editor layer created\n";
}

void EditorLayer::OnDestroy()
{
    std::cerr << "Editor layer destroyed\n";
}

void EditorLayer::OnUpdate(double deltaTime)
{
    std::cerr << "Editor layer updated\n";
}

void EditorLayer::OnUiRender()
{
    std::cerr << "Editor on ui render call\n";
}
