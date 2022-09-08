//
// Created by Anton on 14.08.2022.
//

#include "Global.h"
#include "Config.h"
#include "../UI/UIHandler.h"
#include "../Lighting/ShadowsHandler.h"
#include "../Entity/Entity.h"
#include "../Render/Renderer.h"

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
        case 1:
            return "Lit";
        case 2:
            return "Unlit";
        case 3:
            return "Lighting Only";
        case 4:
            return "Wireframe";
        case 5:
            return "Directional light only";
        case 6:
            return "Point light only";
        case 7:
            return "Specular map view";
        case 8:
            return "Normal map view";
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
        ShadowsHandler::Initialize();
    }
    catch (std::exception& e)
    {
        throw InGameException("Error during program initialization. Reason: " + std::string(e.what()));
    }
    EventsHandler::ToggleCursor();
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

    /* Update window controls */
    Window::Tick();

    auto& c = ResourcesManager::GetPlayerScene()->GetPrimaryCamera().GetComponent<CameraComponent>();
    auto& t = ResourcesManager::GetPlayerScene()->GetPrimaryCamera().GetComponent<TransformComponent>();

    if (EventsHandler::IsPressed(GLFW_KEY_W))
    {
        t.translation += static_cast<float>(deltaTime) * c.camera.front;
    }
    if (EventsHandler::IsPressed(GLFW_KEY_S))
    {
        t.translation -= static_cast<float>(deltaTime) * c.camera.front;
    }
    if (EventsHandler::IsPressed(GLFW_KEY_D))
    {
        t.translation += static_cast<float>(deltaTime) * c.camera.right;
    }
    if (EventsHandler::IsPressed(GLFW_KEY_A))
    {
        t.translation -= static_cast<float>(deltaTime) * c.camera.right;
    }
    if (EventsHandler::IsPressed(GLFW_KEY_Q))
    {
        t.translation += static_cast<float>(deltaTime) * c.camera.up;
    }
    if (EventsHandler::IsPressed(GLFW_KEY_E))
    {
        t.translation -= static_cast<float>(deltaTime) * c.camera.up;
    }

    float mouseSensitivity = 150.0f;
    /* PerspectiveCamera world orientation */
    if (EventsHandler::_cursor_locked)
    {
        c.camera.posX += -EventsHandler::deltaX * deltaTime * mouseSensitivity / (float) Window::GetHeight() * 2;
        c.camera.posY += -EventsHandler::deltaY * deltaTime * mouseSensitivity / (float) Window::GetHeight() * 2;
        if (c.camera.posY < - glm::radians(89.0f))
        {
            c.camera.posY = - glm::radians(89.0f);
        }
        if (c.camera.posY > glm::radians(89.0f))
        {
            c.camera.posY = glm::radians(89.0f);
        }
        c.camera.rotation = glm::vec3(c.camera.posY, c.camera.posX, 0.0f);
        c.camera.Update();
    }


    /* TODO: Move to the Scene class */
    /* Update camera controls */
//    ResourcesManager::GetPlayerScene()->GetPrimaryCamera()->UpdateControls();
}

double Global::GetWorldDeltaTime()
{
    return deltaTime;
}

void Global::Draw()
{
    Shader * uiShader = ResourcesManager::GetShader("uiShader");

    Renderer::Prepare(* ResourcesManager::GetPlayerScene(), drawMode);
    Renderer::Render(* ResourcesManager::GetPlayerScene());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    TransformComponent t = ResourcesManager::GetPlayerScene()->GetPrimaryCamera().GetComponent<TransformComponent>();

    std::string res = "Position" + std::to_string(t.translation.x) + "; " + std::to_string(t.translation.y) + "; " + std::to_string(t.translation.z);
    UIHandler::RenderText(* uiShader, "FPS: " + std::to_string(curFPS), 0.0F, (float) Window::GetHeight() - 24.0F, 1.0, glm::vec3(1.0, 1.0, 1.0));
    UIHandler::RenderText(* uiShader, "View mode: " + drawModeToString(drawMode), 0.0F, (float) Window::GetHeight() - 48.0F, 1.0, glm::vec3(1.0, 1.0, 0.0));
    UIHandler::RenderText(* uiShader, "WASD to move, 1-9 to switch view Modes", 0.0F, (float) Window::GetHeight() - 72.0F, 1.0, glm::vec3(1.0, 1.0, 1.0));
    UIHandler::RenderText(* uiShader, res, 0.0F, (float) Window::GetHeight() - 96.0F, 1.0, glm::vec3(1.0, 1.0, 1.0));
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
