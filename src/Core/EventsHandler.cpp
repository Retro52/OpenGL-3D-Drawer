//
// Created by Anton on 12.12.2021.
//

#include "EventsHandler.h"
#include "Window.h"
#include <memory>
#include <cstring>

constexpr int _MOUSE_BUTTONS_OFFSET = 1024;

std::unique_ptr<bool[]> EventsHandler::_keys;
std::unique_ptr<uint[]> EventsHandler::_frames;
uint EventsHandler::_current = 0;
float EventsHandler::deltaX = 0.0F;
float EventsHandler::deltaY = 0.0F;
float EventsHandler::x = 0.0F;
float EventsHandler::y = 0.0F;
bool EventsHandler::_cursor_locked = false;
bool EventsHandler::_cursor_started = false;


/**
 * Cursor movement callback
 * @param window current glfw window
 * @param xpos mouse x position
 * @param ypos mouse y position
 */
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (EventsHandler::_cursor_started)
    {
        EventsHandler::deltaX += (float) xpos - EventsHandler::x;
        EventsHandler::deltaY += (float) ypos - EventsHandler::y;
    }
    else
    {
        EventsHandler::_cursor_started = true;
    }
    EventsHandler::x = (float) xpos;
    EventsHandler::y = (float) ypos;
}

/**
 * Mouse button click callback, look OpenGL button defines for more
 * @param window current glfw window
 * @param button id of the button
 * @param action id of the action
 * @param mode id of the mode
 */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
    if (action == GLFW_PRESS)
    {
        EventsHandler::_keys[_MOUSE_BUTTONS_OFFSET + button] = true;
        EventsHandler::_frames[_MOUSE_BUTTONS_OFFSET + button] = EventsHandler::_current;
    }
    else if (action == GLFW_RELEASE)
    {
        EventsHandler::_keys[_MOUSE_BUTTONS_OFFSET + button] = false;
        EventsHandler::_frames[_MOUSE_BUTTONS_OFFSET + button] = EventsHandler::_current;
    }
}

/**
 * Key action callback
 * @param window current glfw window
 * @param key key id, look OpenGL defines for more
 * @param scancode scancode id
 * @param action action id
 * @param mode mode id
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (action == GLFW_PRESS)
    {
        EventsHandler::_keys[key] = true;
        EventsHandler::_frames[key] = EventsHandler::_current;
    }
    else if (action == GLFW_RELEASE)
    {
        EventsHandler::_keys[key] = false;
        EventsHandler::_frames[key] = EventsHandler::_current;
    }
}

/**
 * Callback on window size change
 * @param window glfw current window
 * @param width new width
 * @param height new height
 */
void window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0, width, height);
    Window::SetHeight(height);
    Window::SetWidth(width);
    Window::Update();
}

int EventsHandler::Initialize()
{
    GLFWwindow * window = Window::GetCurrentWindow();
    EventsHandler::_keys = std::make_unique<bool[]>(1032);
    EventsHandler::_frames = std::make_unique<uint[]>(1032);
    std::memset(_keys.get(), false, 1032*sizeof(bool));
    std::memset(_frames.get(), 0, 1032*sizeof(uint));

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    return 0;
}

bool EventsHandler::IsPressed(int keycode)
{
    if (keycode < 0 || keycode >= _MOUSE_BUTTONS_OFFSET)
    {
        return false;
    }
    return _keys[keycode];
}

bool EventsHandler::IsJustPressed(int keycode)
{
    if (keycode < 0 || keycode >= _MOUSE_BUTTONS_OFFSET)
    {
        return false;
    }
    return _keys[keycode] && _frames[keycode] == _current;
}

bool EventsHandler::IsClicked(int button)
{
    int index = _MOUSE_BUTTONS_OFFSET + button;
    return _keys[index];
}

bool EventsHandler::IsJustClicked(int button)
{
    int index = _MOUSE_BUTTONS_OFFSET + button;
    return _keys[index] && _frames[index] == _current;
}

void EventsHandler::ToggleCursor()
{
    _cursor_locked = !_cursor_locked;
    Window::SetCursorMode(_cursor_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void EventsHandler::PullEvents()
{
    _current++;
    deltaX = 0.0f;
    deltaY = 0.0f;
    glfwPollEvents();
}