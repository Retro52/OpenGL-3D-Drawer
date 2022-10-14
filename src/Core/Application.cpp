//
// Created by Anton on 27.08.2022.
//

#include "Global.h"
#include "Application.h"
#include "../Render/Renderer.h"

double Application::start = 0.0;

void Application::Create()
{
    auto loadStart = std::chrono::high_resolution_clock::now();
    /* Program initialization */
    try
    {
        Global::Initialize();
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
            Global::Tick();

            Global::Draw();

            /* Swapping buffers and pulling user inputs */
            Global::EndFrame();
        }
    }
    catch(const std::exception& exp)
    {
        LOG(ERROR) << "Exception caught, terminating program. What: " << exp.what();
    }
}

void Application::Destroy()
{
    unsigned long totalFrames = Global::GetTotalFrames();
    double executionTime = glfwGetTime() - start;
    LOG(INFO) << "Window terminated, program finished";
    LOG(INFO) << "Total frames: " << totalFrames;
    LOG(INFO) << "Total execution time: " << executionTime;
    LOG(INFO) << "Average frame time: " << executionTime * 1000.0F / static_cast<double> (totalFrames) << " ms";
    LOG(INFO) << "Average FPS: " << static_cast<double> (totalFrames) / executionTime;
    Window::Terminate();
    Renderer::ShutDown();
}