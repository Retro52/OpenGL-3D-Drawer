//
// Created by Anton on 01.10.2022.
//

#ifndef GRAPHICS_EDITORLAYER_H
#define GRAPHICS_EDITORLAYER_H

#include <filesystem>
#include "../Core/Layer.hpp"

class EditorLayer : public Layer
{
public:
    virtual void OnCreate() override;

    virtual void OnDestroy() override;

    virtual void OnUiRender() override;

    virtual void OnUpdate(double deltaTime) override;

    virtual void OnEvent(const std::shared_ptr<Event>& event) override;

    virtual ~EditorLayer() override = default;

private:
    void LoadImGui();

    void ImGuiNewFrame();

    void DrawImGuiTest();

    void ImGuiEndFrame();

    void ShutImGui();

    template<typename T>
    void AddComponent(const std::string& componentName);

    void DrawEntityProperties(std::unique_ptr<Scene>& scene);

    void OnMouseMoveEvent(const std::shared_ptr<MouseMoveEvent>& event);

    void OnKeyPressedEvent(const std::shared_ptr<KeyPressedEvent>& event);

    void OnKeyReleasedEvent(const std::shared_ptr<KeyReleasedEvent>& event);

    void RenderBrowserPanel(bool& isOpen);

    void RenderSettingsPanel(bool& isOpen);

    void RenderViewportPanel(bool& isOpen, std::unique_ptr<Scene>& scene);

    void RenderEntitiesListPanel(bool& isOpen, std::unique_ptr<Scene>& scene);

private:
    std::filesystem::path curDirectory;
    std::shared_ptr<Mesh> selectedMesh = nullptr;
    std::shared_ptr<Entity> selectedEntity = nullptr;

    int selectedOperation;
};


#endif //GRAPHICS_EDITORLAYER_H
