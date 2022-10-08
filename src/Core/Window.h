//
// Created by Anton on 12.12.2021.
//

#ifndef GRAPHICS_WINDOW_H
#define GRAPHICS_WINDOW_H

#define GLEW_STATIC

#include <memory>
#include <glew.h>
#include <glfw3.h>
#include <string>
#include "../Render/FBO.hpp"
#include "../Render/Material.h"

class Window
{
private:
    static GLFWwindow * window;
    static int width, height;
    static int fboWidth, fboHeight;

    static std::unique_ptr<FBO> windowFBO;
    static std::unique_ptr<Texture> viewportTexture, viewportDepthTexture;
public:
    /* Restriction to create an instance of this class */
    Window() = delete;
    Window(Window&&) = delete;
    Window(const Window&) = delete;

    /**
     * Creates window by given parameters
     * @param w window width
     * @param h window height
     * @param name window name
     * @param fullScreen true if full screen, false otherwise
     */
    static void Initialize(int w, int h, const std::string &name, bool fullScreen = false, int nativeWidth = 1920, int nativeHeight = 1080);

    /**
     * Terminates the window after program is shut
     */
    static void Terminate();

    /**
     * Set cursor mode
     * @param cursorMode gl enum of cursor mode
     */
    static void SetCursorMode(int cursorMode);

    /**
     * Should program close on the next frame
     * @param shouldClose bool var i window should close
     */
    static void SetShouldClose(bool shouldClose);

    /**
     * Is window about to close
     * @return true if yes, false otherwise
     */
    static bool IsShouldClose();

    /**
     * Swap gl window buffers
     */
    static void SwapBuffers();

    /**
     * Get current window
     * @return current window
     */
    static GLFWwindow * GetCurrentWindow();

    /**
     * Set window width
     * @param w new width
     */
    static void SetWidth(int w);

    /**
     * Set window height
     * @param h new height
     */
    static void SetHeight(int h);

    /**
     * Get window width
     * @return window current width
     */
    static int GetWidth();

    /**
     * Get window height
     * @return window height
     */
    static int GetHeight();

    /**
     * Get FBO texture width
     * @return attached to FBO texture`s width
     */
    static int GetFboWidth() { return fboHeight; }

    /**
     * Get FBO texture height
     * @return attached to FBO texture`s height
     */
    static int GetFboHeight() { return fboHeight; }

    static unsigned int GetFboTexture() { return viewportTexture->GetId(); }

    /**
     * Executed every frame
     */
    static void Tick();

    /**
     * Updates window width and height
     */
    static void Update();

    /**
     * @return current window aspect ratio
     */
    inline static float GetAspectRatio() { return static_cast<float> (width) / static_cast<float> (height); }

    static void BindFBO() { windowFBO->Bind(); }
    static void ResetFBO() { windowFBO->Reset(); }
};


#endif //GRAPHICS_WINDOW_H
