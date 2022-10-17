//
// Created by Anton on 01.10.2022.
//

#include "EditorLayer.h"
#include "../Entity/Entity.h"
#include "glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

#include "../Core/Window.h"
#include "../Core/Global.h"
#include "../Input/EventsHandler.h"
#include "../Core/ResourcesManager.h"
#include "../Render/Renderer.h"

void EditorLayer::OnCreate()
{
    LoadImGui();
    curDirectory = std::filesystem::current_path();
}

void EditorLayer::OnDestroy()
{
    ShutImGui();
}

void EditorLayer::OnUpdate(double deltaTime)
{
}

void EditorLayer::OnUiRender()
{
    ImGuiNewFrame();
    DrawImGuiTest();
    ImGuiEndFrame();
}

void EditorLayer::OnEvent(const std::shared_ptr<Event>& event)
{
    switch (event->GetType())
    {
        case EventTypes::KeyPressed:
//            std::cerr << "KeyPressed event\n";
            break;
        case EventTypes::MouseMoved:
//            std::cerr << "Mouse moved event\n";
            break;
        case EventTypes::KeyReleased:
//            std::cerr << "Key released event\n";
            break;
        case EventTypes::WindowResized:
//            std::cerr << "Window resized event\n";
            break;
        case EventTypes::MouseButtonPressed:
//            std::cerr << "ButtonPressed event\n";
            break;
        case EventTypes::MouseButtonReleased:
//            std::cerr << "ButtonReleased event\n";
            break;
    }
}

void EditorLayer::LoadImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(Window::GetCurrentWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void EditorLayer::ImGuiNewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

/* TODO: move every window draw call to separate function */
void EditorLayer::DrawImGuiTest()
{
    static bool isSettingsOpened      = true;
    static bool isViewportOpened      = true;
    static bool isEntitiesListOpened  = true;
    static bool isFolderContentOpened = true;

    ImGui::DockSpaceOverViewport();

    if(ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::Checkbox("Settings", &isSettingsOpened);
            ImGui::Checkbox("Scene viewport", &isViewportOpened);
            ImGui::Checkbox("Entities list", &isEntitiesListOpened);
            ImGui::Checkbox("Folder content", &isFolderContentOpened);
            ImGui::EndMenu();
        }
    }

    auto& scene = ResourcesManager::GetPlayerScene();

    if(isEntitiesListOpened)
    {
        ImGui::Begin("Entities list: ", &isEntitiesListOpened);

        // create new entity
        if (ImGui::BeginPopupContextWindow(nullptr, 1))
        {
            if (ImGui::MenuItem("Create new Entity"))
                scene->CreateEntity("New entity");

            ImGui::EndPopup();
        }

        scene->registry.each([&](auto id)
                             {
                                 std::shared_ptr<Entity> entity = std::make_shared<Entity>(id, scene.get());
                                 auto& nameComponent = entity->template GetComponent<NameComponent>();
                                 std::string label = nameComponent.name + "##" + std::to_string(nameComponent.id.Get());

                                 if(ImGui::Button(label.c_str()))
                                 {
                                     selectedEntity = entity;
                                 }
                             });

        // drop selected entity
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && selectedEntity)
            selectedEntity = nullptr;

        ImGui::End();
    }

    DrawEntityProperties(scene);

    if(isViewportOpened)
    {
        // adding scene viewport
        ImGui::Begin("Scene viewport", &isViewportOpened);
        ImGui::Image(reinterpret_cast<void*>(Renderer::GetRenderedImage()), ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        // drop selected entity
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            EventsHandler::ToggleCursor(true);

        ImGui::End();
    }

    if(isSettingsOpened)
    {
        ImGui::Begin("Settings", &isSettingsOpened);
        ImGui::Text("Frame time: %f ms", Global::GetWorldDeltaTime() * 1000);
        ImGui::Text("Total frames: %ul", Global::GetTotalFrames());
        ImGui::Text("Frame rate: %f FPS", 1.0f / Global::GetWorldDeltaTime());
        ImGui::ColorPicker3("Clear color", (float *)&Renderer::clearColor);
        ImGui::Checkbox("Should apply post process effects", &Renderer::isPostProcessingActivated);

        ImGui::End();
    }

    if(isFolderContentOpened)
    {
        ImGui::Begin("Opened directory:", &isFolderContentOpened);
        ImGui::TextWrapped(curDirectory.string().c_str());
        if(ImGui::Button("<-"))
        {
            curDirectory = curDirectory.parent_path();
        }

        static int buttonsPerRow = 5;

        float buttonWidth = ImGui::GetContentRegionAvail().x / static_cast<float>(buttonsPerRow);
        float buttonHeight = ImGui::GetFontSize() + 5.0f;

        ImGui::Columns(buttonsPerRow, nullptr, false);

        for(const auto& item : std::filesystem::directory_iterator(curDirectory))
        {
            auto itemFilename = item.path().filename().string().c_str();
            ImGui::Button(itemFilename, ImVec2(buttonWidth, buttonHeight));
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                if(item.is_directory())
                {
                    curDirectory = item;
                }
            }
            ImGui::NextColumn();
        }
        ImGui::Columns();
        ImGui::SliderInt("BPR", &buttonsPerRow, 1, 20);
        ImGui::End();
    }
}

void EditorLayer::ImGuiEndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    auto& io = ImGui::GetIO();

    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow * backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup);
    }
}

template<typename T>
void EditorLayer::AddComponent(const std::string& componentName)
{
    if (!selectedEntity->HasComponent<T>())
    {
        if (ImGui::MenuItem(componentName.c_str()))
        {
            selectedEntity->AddComponent<T>();
            ImGui::CloseCurrentPopup();
        }
    }
}

void EditorLayer::ShutImGui()
{
    ImGui::DestroyContext();
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
}

void EditorLayer::DrawEntityProperties(std::unique_ptr<Scene>& scene)
{
    // rendering entity properties
    if(selectedEntity)
    {
        ImGui::Begin("Properties");

        if(ImGui::Button("Delete entity"))
        {
            scene->DeleteEntity(* selectedEntity);
            selectedEntity = nullptr;
            return;
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            AddComponent<CameraComponent>("Camera");
            AddComponent<Model3DComponent>("3D mesh");
            AddComponent<PointLightComponent>("Point light");
            AddComponent<DirectionalLightComponent>("Directional light");
            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        auto& nameComponent = selectedEntity->GetComponent<NameComponent>();
        auto& transformComponent = selectedEntity->GetComponent<TransformComponent>();
        ImGui::InputText("Name", &nameComponent.name);
        ImGui::InputFloat3("Position", (float*)&transformComponent.translation);

        glm::vec3 rotation = glm::degrees(transformComponent.rotation);
        ImGui::SliderFloat3("Rotation", (float*)&rotation, 0.0f, 360.0f);
        transformComponent.rotation = glm::radians(rotation);

        ImGui::InputFloat3("Scale", (float*)&transformComponent.scale);

        if(selectedEntity->HasComponent<Model3DComponent>())
        {
            if(ImGui::CollapsingHeader("Model 3D"))
            {
                auto& modelComponent = selectedEntity->GetComponent<Model3DComponent>();
                ImGui::InputText("Path ", &modelComponent.model.path);

                if (ImGui::Button("Reload model"))
                {
                    try
                    {
                        modelComponent.model = Model(modelComponent.model.path);
                    }
                    catch(const InGameException& e)
                    {
                        LOG(WARNING) << "Failed to reload model. Reason: " << e.what();
                    }

                }


                ImGui::Checkbox("Casts shadow", &modelComponent.castsShadow);
                ImGui::Checkbox("Should be lit", &modelComponent.shouldBeLit);
                ImGui::SliderInt("Tiling factor", &modelComponent.tilingFactor, 1, 100);
            }
        }

        if(selectedEntity->HasComponent<DirectionalLightComponent>())
        {
            if(ImGui::CollapsingHeader("DirectionalLight"))
            {
                auto& dirLightComponent = selectedEntity->GetComponent<DirectionalLightComponent>();
                ImGui::ColorPicker3("Ambient", (float *)&dirLightComponent.directionalLight.ambient);
                ImGui::ColorPicker3("Diffuse", (float *)&dirLightComponent.directionalLight.diffuse);
                ImGui::ColorPicker3("Specular", (float *)&dirLightComponent.directionalLight.specular);
            }
        }

        if(selectedEntity->HasComponent<PointLightComponent>())
        {
            if(ImGui::CollapsingHeader("PointLight"))
            {
                auto& pointLightComponent = selectedEntity->GetComponent<PointLightComponent>();
                ImGui::ColorPicker3("Ambient", (float *)&pointLightComponent.pointLight.ambient);
                ImGui::ColorPicker3("Diffuse", (float *)&pointLightComponent.pointLight.diffuse);
                ImGui::ColorPicker3("Specular", (float *)&pointLightComponent.pointLight.specular);
            }
        }

        if(selectedEntity->HasComponent<CameraComponent>())
        {
            if(ImGui::CollapsingHeader("Camera"))
            {
                auto& cameraComponent = selectedEntity->GetComponent<CameraComponent>();
                ImGui::Checkbox("Is primary", &cameraComponent.isPrimary);
                ImGui::InputFloat2("Aspect ratio", (float *)&cameraComponent.camera.aspectRatio);
                float fov = cameraComponent.camera.GetFieldOfView();
                ImGui::SliderFloat("FOV", &fov, 0, 360);
                cameraComponent.camera.SetFieldOfView(fov);
            }
        }

        ImGui::End();
    }
}
