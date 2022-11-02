//
// Created by Anton on 01.10.2022.
//

#include "EditorLayer.h"
#include "../Entity/Entity.h"
#include "glfw3.h"
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"
#include "ImGuizmo/ImGuizmo.h"

#include "../Core/Window.h"
#include "../Core/Global.h"
#include "../Input/EventsHandler.h"
#include "../Core/Utils.hpp"
#include "../Core/Profiler.hpp"
#include "../Render/Renderer.h"

void EditorLayer::OnCreate()
{
    LoadImGui();
    curDirectory = std::filesystem::current_path();
    selectedOperation = ImGuizmo::OPERATION::TRANSLATE;

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowMinSize        = ImVec2( 160, 20 );
    style.FramePadding         = ImVec2( 4, 2 );
    style.ItemSpacing          = ImVec2( 6, 2 );
    style.ItemInnerSpacing     = ImVec2( 6, 4 );
    style.Alpha                = 0.95f;
    style.WindowRounding       = 4.0f;
    style.FrameRounding        = 2.0f;
    style.IndentSpacing        = 6.0f;
    style.ItemInnerSpacing     = ImVec2( 2, 4 );
    style.ColumnsMinSpacing    = 50.0f;
    style.GrabMinSize          = 14.0f;
    style.GrabRounding         = 16.0f;
    style.ScrollbarSize        = 12.0f;
    style.ScrollbarRounding    = 16.0f;

    style.Colors[ImGuiCol_Text]                  = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Separator]             = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);
    style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
    style.Colors[ImGuiCol_Tab]                   = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style.Colors[ImGuiCol_TabActive]             = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
    style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
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
            OnKeyReleasedEvent(Event::EventCast<KeyReleasedEvent>(event));
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
    ImGuizmo::BeginFrame();
}

/* TODO: move every window draw call to separate function */
void EditorLayer::DrawImGuiTest()
{
    static bool isSettingsOpened      = true;
    static bool isViewportOpened      = true;
    static bool isEntitiesListOpened  = true;
    static bool isFolderContentOpened = true;
    bool isSnappingEnabled = EventsHandler::IsPressed(Key::LeftAlt);

    ImGui::DockSpaceOverViewport();

    if(ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if(ImGui::Button("New"))
            {
                std::string fname = Utils::OpenFile(SCENE_FILE_FILTER);

                if(!fname.empty())
                {
                    ResourcesManager::RegisterPlayerScene(fname);
                }
            }

            if(ImGui::Button("Open"))
            {
                std::string fname = Utils::OpenFile(SCENE_FILE_FILTER);

                if(!fname.empty())
                {
                    ResourcesManager::RegisterPlayerScene(fname);
                    selectedEntity = nullptr;
                    selectedMaterial = nullptr;
                }
            }

            if(ImGui::Button("Save"))
            {
                auto& scene = ResourcesManager::GetPlayerScene();
                scene->SaveScene(scene->path);
            }

            if(ImGui::Button("Save as..."))
            {
                std::string fname = Utils::SaveFile();

                if(!fname.empty())
                {
                    auto& scene = ResourcesManager::GetPlayerScene();
                    scene->SaveScene(fname);
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::Checkbox("Settings", &isSettingsOpened);
            ImGui::Checkbox("Scene viewport", &isViewportOpened);
            ImGui::Checkbox("Entities list", &isEntitiesListOpened);
            ImGui::Checkbox("Folder content", &isFolderContentOpened);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Viewport draw mode"))
        {
            for(int i = 0; i < 10; i++)
            {
                if(ImGui::RadioButton(Renderer::CurDrawModeToString(i).c_str(), Renderer::drawMode == i))
                {
                    Renderer::drawMode = i;
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    auto& scene = ResourcesManager::GetPlayerScene();

    RenderEntitiesListPanel(isEntitiesListOpened, scene);
    DrawEntityProperties(scene);
    RenderViewportPanel(isViewportOpened, scene);
    RenderSettingsPanel(isSettingsOpened);
    RenderBrowserPanel(isFolderContentOpened);
}

void EditorLayer::RenderEntitiesListPanel(bool& isOpen, std::unique_ptr<Scene>& scene)
{
    if( !isOpen ) { return; }
    ImGui::Begin("Entities list: ", &isOpen);

    // create new entity
    if (ImGui::BeginPopupContextWindow(nullptr, 1))
    {
        if (ImGui::BeginMenu("Create new entity"))
        {
            if (ImGui::MenuItem("Empty entity"))
            {
                scene->CreateEntity("New entity");
            }
            if (ImGui::MenuItem("Directional light"))
            {
                auto ent = scene->CreateEntity("Directional light");
                ent.AddComponent<DirectionalLightComponent>();
            }
            if (ImGui::MenuItem("Point light"))
            {
                auto ent = scene->CreateEntity("Point light");
                ent.AddComponent<PointLightComponent>();
            }
            if (ImGui::MenuItem("Mesh"))
            {
                auto ent = scene->CreateEntity("Mesh");
                ent.AddComponent<Model3DComponent>();
            }
            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }

    scene->registry.each([&](auto id)
                         {
                             std::shared_ptr<Entity> entity = std::make_shared<Entity>(id, scene.get());
                             auto& nameComponent = entity->template GetComponent<NameComponent>();
                             std::string label = nameComponent.name + "##" + std::to_string(nameComponent.id.Get());

                             static int entitiesCount = 30;
                             float bWidth  = ImGui::GetWindowWidth();
                             float bHeight = ImGui::GetWindowHeight() / static_cast<float>(entitiesCount);
                             float minHeight = 17.0f;

                             if(ImGui::Button(label.c_str(), ImVec2(bWidth, bHeight > minHeight ? bHeight : minHeight)))
                             {
                                 selectedEntity = entity;
                             }
                         });

    // drop selected entity
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsWindowHovered() && selectedEntity)
    {
        selectedEntity = nullptr;
        selectedMaterial = nullptr;
    }
    else if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && selectedMaterial)
    {
        selectedMaterial = nullptr;
    }


    ImGui::End();
}

void EditorLayer::RenderViewportPanel(bool& isOpen, std::unique_ptr<Scene>& scene)
{
    if( !isOpen ) { return; }

    // adding scene viewport
    ImGui::Begin("Scene viewport", &isOpen);

    ImGui::Image(reinterpret_cast<void*>(Renderer::GetRenderedImage()), ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

    // drop selected entity
    if (ImGui::IsMouseDown(1) && ImGui::IsWindowHovered())
    {
        EventsHandler::ToggleCursor(true);
    }

    if(selectedEntity)
    {
        auto& transformComponent = selectedEntity->GetComponent<TransformComponent>();

        auto primCamera = scene->GetPrimaryCamera();

        if(!primCamera)
        {
            return;
        }

        auto& sceneCamera = primCamera->GetComponent<CameraComponent>().camera;
        auto& sceneCameraPosition = primCamera->GetComponent<TransformComponent>().translation;

        auto selEntityTransform = transformComponent.GetTransform();

        glm::mat4 cameraProj = sceneCamera.GetProjection();
        glm::mat4 cameraView = sceneCamera.GetView(sceneCameraPosition);

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

        static float deltaMove = 1.0f;

        switch((ImGuizmo::OPERATION) selectedOperation)
        {
            case ImGuizmo::OPERATION::ROTATE:
                deltaMove = 5.0f;
                break;
            case ImGuizmo::OPERATION::TRANSLATE:
                deltaMove = 0.5f;
                break;
            case ImGuizmo::OPERATION::SCALE:
                deltaMove = 0.1f;
                break;
            default:
                break;
        }


        float snapVals[3] = {deltaMove, deltaMove, deltaMove};

        ImGuizmo::Manipulate(glm::value_ptr(cameraView),
                             glm::value_ptr(cameraProj),
                             (ImGuizmo::OPERATION) selectedOperation,
                             ImGuizmo::MODE::WORLD,
                             glm::value_ptr(selEntityTransform),
                             nullptr,
                             EventsHandler::IsPressed(Key::LeftAlt) ? snapVals : nullptr);

        if (ImGuizmo::IsUsing())
        {
            glm::vec3 translation, rotation, scale;
            ImGuizmo::DecomposeMatrixToComponents(
                    glm::value_ptr(selEntityTransform),
                    glm::value_ptr(translation),
                    glm::value_ptr(rotation),
                    glm::value_ptr(scale)
            );

            auto deltaRot = rotation - transformComponent.rotation;

            transformComponent.translation = translation;
            transformComponent.rotation = rotation;
            transformComponent.scale = scale;
        }
    }

    ImGui::End();
}

void EditorLayer::RenderSettingsPanel(bool& isOpen)
{
    if( !isOpen ) { return; }

    float pTime = static_cast<float>(Profiler::prepTimer.duration<std::chrono::nanoseconds>().count()) / 1000000.0f;
    float cTime = static_cast<float>(Profiler::cpuTimer.duration<std::chrono::nanoseconds>().count()) / 1000000.0f;
    float gTime = static_cast<float>(Profiler::gTimer.duration<std::chrono::nanoseconds>().count())   / 1000000.0f;
    float sTime = static_cast<float>(Profiler::sTimer.duration<std::chrono::nanoseconds>().count())   / 1000000.0f;
    float lTime = static_cast<float>(Profiler::lTimer.duration<std::chrono::nanoseconds>().count())   / 1000000.0f;
    float ctTotal = cTime + gTime + sTime + lTime + pTime;

    ImGui::Begin("Settings", &isOpen);
    ImGui::Separator();
    ImGui::Text("Current resolution: %u x %u", Renderer::GetFboWidth(), Renderer::GetFboHeight());
    ImGui::Text("Total frames: %ul", Global::GetTotalFrames());
    ImGui::Text("Frame rate: %f FPS", 1.0f / Global::GetWorldDeltaTime());
    ImGui::Text("Total meshes:               %u", Profiler::totalMeshes);
    ImGui::Text("Total vertices:             %u", Profiler::totalVertices);
    ImGui::Text("Update time (ms):           %f", cTime);
    ImGui::Text("Prep time:                  %f", pTime);
    ImGui::Text("G-pass time (ms):           %f", gTime);
    ImGui::Text("Shadow rendering time (ms): %f", sTime);
    ImGui::Text("L-pass time (ms):           %f", lTime);
    ImGui::Separator();
    ImGui::Text("Total CPU time:             %f", ctTotal);
    ImGui::Separator();
    ImGui::Text("ImGui and SwapBuffers:      %f", Global::GetWorldDeltaTime() * 1000 - ctTotal);
    ImGui::Separator();
    ImGui::Text("Total frame time (s):       %f", Global::GetWorldDeltaTime() * 1000);
    ImGui::Separator();
    ImGui::Checkbox("Should draw final results to the FBO (Shift + F2)", &Renderer::shouldDrawFinalToFBO);
    ImGui::Checkbox("Should apply post process effects", &Renderer::isPostProcessingActivated);

    ImGui::Separator();
    ImGui::Text("Gizmos current operation:");

    if(ImGui::RadioButton("Move", selectedOperation == ImGuizmo::OPERATION::TRANSLATE))
    {
        selectedOperation = ImGuizmo::OPERATION::TRANSLATE;
    }

    if(ImGui::RadioButton("Rotate", selectedOperation == ImGuizmo::OPERATION::ROTATE))
    {
        selectedOperation = ImGuizmo::OPERATION::ROTATE;
    }

    if(ImGui::RadioButton("Scale", selectedOperation == ImGuizmo::OPERATION::SCALE))
    {
        selectedOperation = ImGuizmo::OPERATION::SCALE;
    }

    ImGui::Text("Is snapping enabled: ");
    ImGui::SameLine();
    if (EventsHandler::IsPressed(Key::LeftAlt))
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.24f, 1.0f), "ON");
    }
    else
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "OFF");
    }

    ImGui::Separator();
    ImGui::Text("Lighting settings:");

    if(ImGui::RadioButton("Dynamic", Renderer::lightingType == LightingType::Dynamic))
    {
        Renderer::lightingType = LightingType::Dynamic;
    }

    if(ImGui::RadioButton("Baked", Renderer::lightingType == LightingType::Baked))
    {
        Renderer::lightingType = LightingType::Baked;
    }

    if(Renderer::lightingType == LightingType::Baked && ImGui::Button("Bake lighting"))
    {
        LOG(DEBUG) << "Lighting baking requested";
    }

    ImGui::SliderFloat("Base offset", &Renderer::baseOffset, 0.0f, 100.0f);
    ImGui::SliderFloat("Delta offset", &Renderer::deltaOffset, 0.0f, 100.0f);
    ImGui::SliderFloat("Factor multiplier", &Renderer::factorMultiplier, 0.0f, 100.0f);
    for(int i = 0; i < Renderer::GetCascadesCount(); i++)
    {
        ImGui::Text("Cascade level[%d]", i);
        ImGui::InputFloat(("##cascade_level_" + std::to_string(i)).c_str(), &Renderer::cascadeLevels.at(i));
    }
    ImGui::End();
}

void EditorLayer::RenderBrowserPanel(bool& isOpen)
{
    if( !isOpen ) { return; }

    ImGui::Begin("Opened directory:", &isOpen);
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
        std::string itemFilenameStr = item.path().filename().string();
        const char * itemFilename = itemFilenameStr.c_str();
        ImGui::Button(itemFilename, ImVec2(buttonWidth, buttonHeight));
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            if(item.is_directory())
            {
                curDirectory = item;
            }
        }
        if (ImGui::BeginDragDropSource() && !item.is_directory())
        {
            auto relPathStr = std::filesystem::relative(item.path()).string();
            auto relPath = relPathStr.c_str();

            if(relPathStr.length() > 32)
            {
                LOG(WARNING) << "Path is too long\n";
            }

            // adding one to catch null terminator to the passed string
            ImGui::SetDragDropPayload("FILE_SELECTED", relPath, (relPathStr.length() + 1) * sizeof(const char));
            ImGui::EndDragDropSource();
        }

        ImGui::NextColumn();
    }
    ImGui::Columns();
    ImGui::SliderInt("BPR", &buttonsPerRow, 1, 20);
    ImGui::End();
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
            selectedMaterial = nullptr;
            return;
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("AddComponent");
        }

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
        ImGui::DragFloat3("Position", (float*)&transformComponent.translation, 0.1);

        glm::vec3 rotation = glm::degrees(transformComponent.rotation);
        ImGui::SliderFloat3("Rotation", (float*)&rotation, 0.0f, 360.0f);
        transformComponent.rotation = glm::radians(rotation);

        ImGui::DragFloat3("Scale", (float*)&transformComponent.scale, 0.05);

        if(selectedEntity->HasComponent<Model3DComponent>())
        {
            if(ImGui::CollapsingHeader("Model 3D"))
            {
                bool shouldDelete = false;
                if (ImGui::Button("Delete component"))
                {
                    shouldDelete = true;
                }

                auto& modelComponent = selectedEntity->GetComponent<Model3DComponent>();
                ImGui::Text("Path: %s", modelComponent.model.path.c_str());
                if(ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_SELECTED"))
                    {
                        if(payload->Data)
                        {
                            modelComponent.model.path = std::string((char *) payload->Data);
                        }
                        else
                        {
                            LOG(WARNING) << " Accepted payload of type FILE_SELECTED is nullptr\n";
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

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

                if(ImGui::CollapsingHeader("Materials: "))
                {
                    for (const auto& mesh : modelComponent.model.meshes)
                    {
                        if(ImGui::Button(mesh->name.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 16)))
                        {
                            selectedMaterial = mesh;
                        }
                        ImGui::Separator();
                    }
                }

                if(shouldDelete)
                {
                    selectedEntity->RemoveComponent<Model3DComponent>();
                    selectedMaterial = nullptr;
                }
            }
        }

        if(selectedEntity->HasComponent<DirectionalLightComponent>())
        {
            if(ImGui::CollapsingHeader("DirectionalLight"))
            {
                bool shouldDelete = false;
                if (ImGui::Button("Delete component"))
                {
                    shouldDelete = true;
                }

                auto& dirLightComponent = selectedEntity->GetComponent<DirectionalLightComponent>();
                ImGui::ColorEdit3("Ambient", (float *)&dirLightComponent.directionalLight.ambient);
                ImGui::ColorEdit3("Diffuse", (float *)&dirLightComponent.directionalLight.diffuse);
                ImGui::ColorEdit3("Specular", (float *)&dirLightComponent.directionalLight.specular);

                if(shouldDelete)
                {
                    selectedEntity->RemoveComponent<Model3DComponent>();
                    selectedMaterial = nullptr;
                }
            }
        }

        if(selectedEntity->HasComponent<PointLightComponent>())
        {
            if(ImGui::CollapsingHeader("PointLight"))
            {
                bool shouldDelete = false;
                if (ImGui::Button("Delete component"))
                {
                    shouldDelete = true;
                }

                auto& pointLightComponent = selectedEntity->GetComponent<PointLightComponent>();
                ImGui::ColorEdit3("Ambient", (float *)&pointLightComponent.pointLight.ambient);
                ImGui::ColorEdit3("Diffuse", (float *)&pointLightComponent.pointLight.diffuse);
                ImGui::ColorEdit3("Specular", (float *)&pointLightComponent.pointLight.specular);

                if(shouldDelete)
                {
                    selectedEntity->RemoveComponent<Model3DComponent>();
                }
            }
        }

        if(selectedEntity->HasComponent<CameraComponent>())
        {
            if(ImGui::CollapsingHeader("Camera"))
            {
                bool shouldDelete = false;
                if (ImGui::Button("Delete component"))
                {
                    shouldDelete = true;
                }

                auto& cameraComponent = selectedEntity->GetComponent<CameraComponent>();
                ImGui::Checkbox("Is primary", &cameraComponent.isPrimary);
                ImGui::InputFloat2("Aspect ratio", (float *)&cameraComponent.camera.aspectRatio);
                float fov = glm::degrees(cameraComponent.camera.GetFieldOfView());
                ImGui::SliderFloat("FOV", &fov, 0, 360);
                cameraComponent.camera.SetFieldOfView(glm::radians(fov));

                if(shouldDelete)
                {
                    selectedEntity->RemoveComponent<Model3DComponent>();
                }
            }
        }

        ImGui::End();
    }

    if(selectedMaterial)
    {
        ImGui::Begin("Material properties");
        ImGui::InputText("Name: ", &selectedMaterial->name);
        ImGui::Separator();

        auto& material = selectedMaterial->material;
        ImGui::SliderFloat("Specular: ", &material.specular, 0.0, 1.0);
        ImGui::ColorEdit3("Material color: ", (float *) &material.defaultColor);

        ImGui::Text("Diffuse texture");
        RenderMaterialTextures(material.materialTextures.at(TextureType::Diffuse));

        ImGui::Text("Normal texture");
        RenderMaterialTextures(material.materialTextures.at(TextureType::Normal));

        ImGui::Text("Specular texture");
        RenderMaterialTextures(material.materialTextures.at(TextureType::Specular));

        ImGui::End();
    }
}

void EditorLayer::RenderMaterialTextures(TextureStack& stack)
{
    if(stack.empty())
    {
        ImGui::PushID(&stack);
        if(ImGui::Button("+"))
        {
            stack.push_back(std::make_shared<Texture>(64, 64));
        }
    }
    for (auto i = stack.begin(); i != stack.end();)
    {
        auto& texture = * i;
        ImGui::Image(reinterpret_cast<void*>(texture->GetId()), ImVec2(64, 64));
        ImGui::SameLine();
        auto path = texture->GetPath();
        if(path.empty())
        {
            ImGui::Text("Empty texture");
        }
        else
        {
            ImGui::Text(texture->GetPath().c_str());
        }

        if(ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("FILE_SELECTED"))
            {
                if (payload->Data)
                {
                    bool loaded = false;
                    for (const auto &tex: Texture::texturesLoaded)
                    {
                        if (*tex == ((char *) payload->Data))
                        {
                            loaded = true;
                            texture = tex;
                            break;
                        }
                    }
                    if (!loaded)
                    {
                        texture = std::make_shared<Texture>((char *) payload->Data);
                    }
                }
                else
                {
                    LOG(WARNING) << " Accepted payload of type FILE_SELECTED is nullptr\n";
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::PushID(&stack + 1);
        if(ImGui::Button("-"))
        {
            i = stack.erase(i);
        }
        else
        {
            i++;
        }
    }
}

void EditorLayer::OnKeyReleasedEvent(const std::shared_ptr<KeyReleasedEvent> &event)
{
    bool isShiftPressed = EventsHandler::IsPressed(Key::LeftShift);
    bool isControlPressed = EventsHandler::IsPressed(Key::LeftControl);
    switch (event->GetKeyCode())
    {
        case Key::Space:
            if (selectedOperation == ImGuizmo::OPERATION::TRANSLATE)
            {
                selectedOperation = ImGuizmo::OPERATION::ROTATE;
                break;
            }
            if (selectedOperation == ImGuizmo::OPERATION::ROTATE)
            {
                selectedOperation = ImGuizmo::OPERATION::SCALE;
                break;
            }
            if (selectedOperation == ImGuizmo::OPERATION::SCALE)
            {
                selectedOperation = ImGuizmo::OPERATION::TRANSLATE;
                break;
            }
        case Key::D:
            if (!isControlPressed || EventsHandler::_cursor_locked)
            {
                break;
            }
            if(selectedEntity)
            {
                selectedEntity = std::make_shared<Entity>(selectedEntity->CopyEntity());
            }
            break;
        default:
            break;
    }
}
