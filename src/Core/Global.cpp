//
// Created by Anton on 14.08.2022.
//

#include "Global.h"
#include "Config.h"
#include "../UI/UIHandler.h"
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

    /* Update window controls */
    Window::Tick();
    ResourcesManager::GetPlayerScene()->OnUpdate(deltaTime);

    auto& tickEvents = EventsStack::GetEvents();
    while (!tickEvents.empty())
    {
        std::cerr << "Layers update\n";
        for(const auto& layer : ResourcesManager::GetLayers())
        {
            layer->OnEvent(tickEvents.front());
        }
        tickEvents.pop();
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
    Renderer::Render(* ResourcesManager::GetPlayerScene(), shadowTexture);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    UIHandler::RenderText(uiShader, "FPS: " + std::to_string(curFPS), 0.0F, (float) Window::GetHeight() - 48.0F, 16);
    UIHandler::RenderText(uiShader, "View mode: " + drawModeToString(drawMode), 0.0F, (float) Window::GetHeight() - 64.0F, 16, glm::vec3(1.0, 1.0, 0.0));

    auto selEnt = ResourcesManager::GetPlayerScene()->GetSelectedEntity();
    auto& selEntTransform = selEnt.GetComponent<TransformComponent>();
    auto& selEntNameComp = selEnt.GetComponent<NameComponent>();

    std::stringstream trans;
    std::stringstream rot;
    std::stringstream sc;
    trans << "Translation: " << std::to_string(selEntTransform.translation.x) << "; " << std::to_string(selEntTransform.translation.y) << "; " << std::to_string(selEntTransform.translation.z);
    rot << "Rotation: " << std::to_string(glm::degrees(selEntTransform.rotation.x))  << "; " << std::to_string(glm::degrees(selEntTransform.rotation.y))  << "; " << std::to_string(glm::degrees(selEntTransform.rotation.z));
    sc << "Scale: " << std::to_string(selEntTransform.scale.x)  << "; " << std::to_string(selEntTransform.scale.y)  << "; " << std::to_string(selEntTransform.scale.z);
    UIHandler::RenderText(uiShader, "Name: " + selEntNameComp.name, 0.0F, (float) Window::GetHeight() - 80.0F, 16, glm::vec3(1.0, 1.0, 0.0));
    UIHandler::RenderText(uiShader, trans.str(), 0.0F, (float) Window::GetHeight() - 96.0F, 16, glm::vec3(1.0, 1.0, 0.0));
    UIHandler::RenderText(uiShader, rot.str(), 0.0F, (float) Window::GetHeight() - 112.0F, 16, glm::vec3(1.0, 1.0, 0.0));
    UIHandler::RenderText(uiShader, sc.str(), 0.0F, (float) Window::GetHeight() - 128.0F, 16, glm::vec3(1.0, 1.0, 0.0));

}

void Global::EndFrame()
{
    /* Swapping OpenGL buffers and pulling this frame events */
    Window::SwapBuffers();
    EventsHandler::PullEvents();
}

unsigned long Global::GetTotalFrames()
{
    return frames;
}