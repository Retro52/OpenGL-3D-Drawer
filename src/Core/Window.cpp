//
// Created by Anton on 12.12.2021.
//
#define GLEW_STATIC

#include "Window.h"
#include "../Input/EventsHandler.h"
#include "InGameException.h"
#include <iostream>
#include <thread>

int Window::width = 0;
int Window::height = 0;
int Window::fboWidth = 2048;
int Window::fboHeight = 2048;
GLFWwindow * Window::window;
std::unique_ptr<FBO> Window::windowFBO;
std::unique_ptr<Texture> Window::viewportTexture, Window::viewportDepthTexture;


void Window::Initialize(int w, int h, const std::string &name, bool fullScreen, int nativeWidth, int nativeHeight)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    if (fullScreen)
    {
        Window::window = glfwCreateWindow(w, h, name.c_str(), glfwGetPrimaryMonitor(), nullptr);
        glfwSetWindowSize(window, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
    }
    else
    {
        Window::window = glfwCreateWindow(w, h, name.c_str(), nullptr, nullptr);
    }

    if (Window::window == nullptr)
    {
        glfwTerminate();
        throw InGameException("Failure during GLFW window creation");
    }

    glfwMakeContextCurrent(Window::window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        throw InGameException("Failure during GLEW initialization");
    }

//    fboWidth = nativeWidth;
//    fboHeight = nativeHeight;

    windowFBO = std::make_unique<FBO>();
    windowFBO->Bind();

    viewportTexture = std::make_unique<Texture>(
            fboWidth,
            fboHeight,
            GL_RGB,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            false
            );
    viewportDepthTexture = std::make_unique<Texture>(
            fboWidth,
            fboHeight,
            GL_DEPTH_STENCIL,
            GL_DEPTH24_STENCIL8,
            GL_UNSIGNED_INT_24_8,
            false
            );

    windowFBO->AddTexture(viewportTexture->GetId(), GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0);
    windowFBO->AddTexture(viewportDepthTexture->GetId(), GL_TEXTURE_2D, GL_DEPTH_STENCIL_ATTACHMENT);

    GAME_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "WINDOW::ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    windowFBO->Reset();

    glViewport(0, 0, fboWidth, fboHeight);
    glClearColor(0.0f,0.0f,0.0f,1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glfwSwapInterval(0);
    Window::width = w;
    Window::height = h;
}

void Window::Terminate()
{
    glfwTerminate();
}

void Window::SetCursorMode(int cursorMode)
{
    glfwSetInputMode(Window::window, GLFW_CURSOR, cursorMode);
}

bool Window::IsShouldClose()
{
    return glfwWindowShouldClose(Window::window) != 0;
}

void Window::SetShouldClose(bool shouldClose)
{
    glfwSetWindowShouldClose(Window::window, shouldClose);
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(Window::window);
}

int Window::GetWidth()
{
    return Window::width;
}

int Window::GetHeight()
{
    return Window::height;
}

void Window::SetWidth(int w)
{
    if (400 < w && w < 3840)
    {
        Window::width = w;
    }
}

void Window::SetHeight(int h)
{
    if (400 < h && h < 2160)
    {
        Window::height = h;
    }
}

GLFWwindow * Window::GetCurrentWindow()
{
    return Window::window;
}

void Window::Tick()
{
    if (EventsHandler::IsJustPressed(GLFW_KEY_ESCAPE))
    {
        Window::SetShouldClose(true);
    }
    /* Show/hide cursor */
    if ((EventsHandler::IsJustPressed(GLFW_KEY_F1) && EventsHandler::IsPressed(GLFW_KEY_LEFT_SHIFT)) || EventsHandler::IsPressed(GLFW_KEY_TAB) && EventsHandler::IsJustPressed(GLFW_KEY_F1))
    {
        EventsHandler::ToggleCursor();
    }
}

void Window::Update()
{
    glfwSetWindowSize(window, width, height);
}

