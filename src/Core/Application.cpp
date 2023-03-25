//
// Created by Anton on 27.08.2022.
//

#include "MainLoop.h"
#include "ResourcesManager.h"
#include "Application.h"
#include "../Render/Renderer.h"

void Application::Init()
{
    auto loadStart = std::chrono::high_resolution_clock::now();

    /* Program initialization */
    try
    {
        MainLoop::Initialize();
        LOG(INFO) << "Program successfully loaded in " << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::high_resolution_clock::now() - loadStart).count() / 1000.0F << " seconds";
        start = glfwGetTime();
    }
    catch(std::exception& e)
    {
        LOG(ERROR) << "Fatal error. Reason: " << e.what();
        LOG(FATAL) << "Program failed in " << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::high_resolution_clock::now() - loadStart).count() / 1000.0F << " seconds";
    }
}

void Application::Run()
{
    try
    {
        /* Tick event */
        while (!Window::IsShouldClose())
        {
            /* Global tick events */
            MainLoop::Tick();

            MainLoop::Draw();

            /* Swapping buffers and pulling user inputs */
            MainLoop::EndFrame();
        }
    }
    catch(const std::exception& exp)
    {
        LOG(ERROR) << "Exception caught, terminating program. What: " << exp.what();
    }
}

void Application::Destroy()
{
    unsigned long totalFrames = MainLoop::GetTotalFrames();
    double executionTime = glfwGetTime() - start;
    LOG(INFO) << "Window terminated, program finished";
    LOG(INFO) << "Total frames: " << totalFrames;
    LOG(INFO) << "Total execution time: " << executionTime;
    LOG(INFO) << "Average frame time: " << executionTime * 1000.0F / static_cast<double> (totalFrames) << " ms";
    LOG(INFO) << "Average FPS: " << static_cast<double> (totalFrames) / executionTime;

    Renderer::ShutDown();
    ResourcesManager::ShutDown();

    Window::Terminate();
}