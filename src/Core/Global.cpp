//
// Created by Anton on 14.08.2022.
//

#include "imgui.h"
#include "Global.h"
#include "Config.h"
#include "../Entity/Entity.h"
#include "../Render/Renderer.h"
#include "../Editor/EditorLayer.h"
#include "../Lighting/ShadowsHandler.h"

double Global::lastTime;
double Global::deltaTime;
double Global::elapsedTime;


int Global::curFPS, Global::drawMode = 1;


unsigned long Global::frame = 0;
unsigned long Global::frames = 0;

std::string drawModeToString(int drawMode)
{
    switch (drawMode)
    {
        case 0:
            return "";
        case 1:
            return "Lit";
        case 2:
            return "Lighting only";
        case 3:
            return "Directional lighting only";
        case 4:
            return "Point lighting only";
        case 5:
            return "Diffuse map view";
        case 6:
            return "Normal map view";
        case 7:
            return "Specular map view";
        case 8:
            return "";
        case 9:
            return "Roughness map view";
        default:
            return std::to_string(drawMode);
    }
}

void Global::Initialize()
{
    lastTime = 0.0;
    deltaTime = 0.0;
    elapsedTime = 0.0;
    curFPS = 0;

    LOG(INFO) << "Program initialization started";

    try
    {
        Config::LoadIni("config.ini");
        EventsHandler::Initialize();
        ShadowsHandler::Initialize(4);
        Renderer::Initialize();
    }
    catch (std::exception& e)
    {
        throw InGameException("Error during program initialization. Reason: " + std::string(e.what()));
    }
    EventsHandler::ToggleCursor();
    ResourcesManager::RegisterLayer(std::make_shared<EditorLayer>());
}

void Global::Tick()
{
    frame++;
    frames++;
    /* Updating delta time */
    double curTime = glfwGetTime();
    deltaTime = curTime - lastTime;
    lastTime = curTime;
    elapsedTime += deltaTime;

    if (elapsedTime >= 0.33)
    {
        curFPS = (int) ((double) frame / elapsedTime);
        frame = 0;
        elapsedTime = 0.0;
    }

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
    /* Debug features */
    if(EventsHandler::IsJustPressed(GLFW_KEY_0))
    {
        drawMode = 0;
    }
    if(EventsHandler::IsJustPressed(GLFW_KEY_1))
    {
        drawMode = 1;
    }
    if (EventsHandler::IsJustPressed(GLFW_KEY_2))
    {
        drawMode = 2;
    }
    if (EventsHandler::IsJustPressed(GLFW_KEY_3))
    {
        drawMode = 3;
    }
    if (EventsHandler::IsJustPressed(GLFW_KEY_4))
    {
        drawMode = 4;
    }
    if (EventsHandler::IsJustPressed(GLFW_KEY_5))
    {
        drawMode = 5;
    }
    if (EventsHandler::IsJustPressed(GLFW_KEY_6))
    {
        drawMode = 6;
    }
    if (EventsHandler::IsJustPressed(GLFW_KEY_7))
    {
        drawMode = 7;
    }
    if (EventsHandler::IsJustPressed(GLFW_KEY_8))
    {
        drawMode = 8;
    }
    if (EventsHandler::IsJustPressed(GLFW_KEY_9))
    {
        drawMode = 9;
    }
    if(EventsHandler::IsJustPressed(GLFW_KEY_R))
    {
        ResourcesManager::RegisterPlayerScene("../res/scenes/defaultScene.json");
    }
}

double Global::GetWorldDeltaTime()
{
    return deltaTime;
}

void Global::Draw()
{
    std::shared_ptr<Shader>& uiShader = ResourcesManager::GetShader("uiShader");

    Renderer::Prepare(* ResourcesManager::GetPlayerScene(), drawMode);
    unsigned int shadowTexture = ShadowsHandler::RenderShadowMap();
    Window::BindFBO();
    glClearColor(0.203f, 0.76f, 0.938f,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if(drawMode == 4)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    Renderer::Render(* ResourcesManager::GetPlayerScene(), shadowTexture);
    Window::ResetFBO();
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
    return frames;
}