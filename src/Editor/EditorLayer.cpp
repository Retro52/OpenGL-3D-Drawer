//
// Created by Anton on 01.10.2022.
//

#include "EditorLayer.h"
#include "glfw3.h"

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

void EditorLayer::OnEvent(const std::shared_ptr<Event>& event)
{
    switch (event->GetType())
    {
        case EventTypes::KeyPressed:
            std::cerr << "KeyPressed event\n";
            break;
        case EventTypes::MouseMoved:
//            std::cerr << "Mouse moved event\n";
            break;
        case EventTypes::KeyReleased:
            std::cerr << "Key released event\n";
            break;
        case EventTypes::WindowResized:
            std::cerr << "Window resized event\n";
            break;
        case EventTypes::MouseButtonPressed:
            std::cerr << "ButtonPressed event\n";
            break;
        case EventTypes::MouseButtonReleased:
            std::cerr << "ButtonReleased event\n";
            break;
    }
}
