//
// Created by Anton on 14.08.2022.
//

#include "imgui.h"
#include "Global.h"
#include "Config.h"
#include "../Entity/Entity.h"
#include "../Render/Renderer.h"
#include "../Editor/EditorLayer.h"

double Global::lastTime;
double Global::deltaTime;


unsigned long Global::totalFrames = 0;


void Global::Initialize()
{
    lastTime = 0.0;
    deltaTime = 0.0;

    LOG(INFO) << "Program initialization started";

    try
    {
        Config::LoadIni("config.ini");
        EventsHandler::Initialize();
        Renderer::Initialize();
    }
    catch (std::exception& e)
    {
        throw InGameException("Error during program initialization. Reason: " + std::string(e.what()));
    }
    ResourcesManager::RegisterLayer(std::make_shared<EditorLayer>());

    EventsHandler::ToggleCursor(true);
}

void Global::Tick()
{
    totalFrames++;
    /* Updating delta time */
    double curTime = glfwGetTime();
    deltaTime = curTime - lastTime;
    lastTime = curTime;

    /* Update window controls */
    Window::Tick();
    ResourcesManager::GetPlayerScene()->OnUpdate(deltaTime);

    auto& tickEvents = EventsStack::GetEvents();
    while (!tickEvents.empty())
    {
        for(const auto& layer : ResourcesManager::GetLayers())
        {
            layer->OnEvent(tickEvents.front());
        }
        tickEvents.pop();
    }

    if(!EventsHandler::_cursor_locked)
    {
        return;
    }
}

double Global::GetWorldDeltaTime()
{
    return deltaTime;
}

void Global::Draw()
{
    auto& curScene = * ResourcesManager::GetPlayerScene();
    /* Preparing renderer for the new frame */
    Renderer::Prepare(curScene);

    if(Renderer::lightingType == LightingType::Dynamic)
    {
        /* Rendering shadow texture */
        Renderer::RenderShadowMaps(curScene);
    }

    if(!Renderer::shouldDrawFinalToFBO && !Renderer::isPostProcessingActivated)
    {
        FBO::Reset();
    }
    else
    {
        /* Binding viewport FBO */
        Renderer::GetViewportFBO()->Bind();
    }

    Renderer::Clear();
    Renderer::EnableDepthTesting();


    Renderer::Render(curScene);

    if(Renderer::isPostProcessingActivated)
    {
        Renderer::ApplyPostProcessing();
    }
}

void Global::EndFrame()
{
    for(const auto& layer : ResourcesManager::GetLayers())
    {
        layer->OnUiRender();
    }

    /* Swapping OpenGL buffers and pulling this frame events */
    Window::SwapBuffers();
    EventsHandler::PullEvents();
}

unsigned long Global::GetTotalFrames()
{
    return totalFrames;
}