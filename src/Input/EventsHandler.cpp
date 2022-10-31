//
// Created by Anton on 12.12.2021.
//

#include "EventsHandler.h"
#include "../Core/Window.h"
#include "../Core/Event.hpp"
#include <array>
#include <iostream>

std::array<bool, 1032> EventsHandler::_keys;
std::array<uint, 1032> EventsHandler::_frames;
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
void EventsHandler::cursor_position_callback(GLFWwindow * /* window */, double xpos, double ypos)
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

    EventsStack::Emplace(std::make_shared<MouseMoveEvent>(deltaX, deltaY));
}

/**
 * Mouse button click callback, look OpenGL button defines for more
 * @param window current glfw window
 * @param button id of the button
 * @param action id of the action
 * @param mode id of the mode
 */
void EventsHandler::mouse_button_callback(GLFWwindow * /* window */, int button, int action, int /* mode */)
{
    if (MOUSE_BUTTONS_OFFSET + button > 1032 || button < 0)
    {
        return;
    }
    if (action == GLFW_PRESS)
    {
        EventsHandler::_keys.at(MOUSE_BUTTONS_OFFSET + button) = true;
        EventsHandler::_frames.at(MOUSE_BUTTONS_OFFSET + button) = EventsHandler::_current;

        EventsStack::Emplace(std::make_shared<MouseButtonClick>(button));
    }
    else if (action == GLFW_RELEASE)
    {
        EventsHandler::_keys.at(MOUSE_BUTTONS_OFFSET + button) = false;
        EventsHandler::_frames.at(MOUSE_BUTTONS_OFFSET + button) = EventsHandler::_current;

        EventsStack::Emplace(std::make_shared<MouseButtonRelease>(button));
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
void EventsHandler::key_callback(GLFWwindow * /* window */, int key, int /* scancode */ , int action, int /* mode */)
{
    if (key > MOUSE_BUTTONS_OFFSET || key < 0)
    {
        return;
    }
    if (action == GLFW_PRESS)
    {
        EventsHandler::_keys.at(key) = true;
        EventsHandler::_frames.at(key) = EventsHandler::_current;
        EventsStack::Emplace(std::make_shared<KeyPressedEvent>(key));
    }
    else if (action == GLFW_RELEASE)
    {
        EventsHandler::_keys.at(key) = false;
        EventsHandler::_frames.at(key) = EventsHandler::_current;
        EventsStack::Emplace(std::make_shared<KeyReleasedEvent>(key));
    }
}

/**
 * Callback on window size change
 * @param window glfw current window
 * @param width new width
 * @param height new height
 */
void EventsHandler::window_size_callback(GLFWwindow * /*window*/, int width, int height)
{
    glViewport(0,0, width, height);
    Window::SetHeight(height);
    Window::SetWidth(width);
    Window::Update();

    EventsStack::Emplace(std::make_shared<WindowResizeEvent>(width, height));
}

int EventsHandler::Initialize()
{
    GLFWwindow * window = Window::GetCurrentWindow();

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    return 0;
}

bool EventsHandler::IsPressed(int keycode)
{
    if (keycode < 0 || keycode >= MOUSE_BUTTONS_OFFSET)
    {
        return false;
    }
    return _keys.at(keycode);
}

bool EventsHandler::IsJustPressed(int keycode)
{
    if (keycode < 0 || keycode >= MOUSE_BUTTONS_OFFSET)
    {
        return false;
    }
    return _keys.at(keycode) && _frames.at(keycode) == _current;
}

bool EventsHandler::IsClicked(int button)
{
    int index = MOUSE_BUTTONS_OFFSET + button;
    return _keys.at(index);
}

bool EventsHandler::IsJustClicked(int button)
{
    int index = MOUSE_BUTTONS_OFFSET + button;
    return _keys.at(index) && _frames.at(index) == _current;
}

void EventsHandler::ToggleCursor()
{
    _cursor_locked = !_cursor_locked;
    Window::SetCursorMode(_cursor_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void EventsHandler::ToggleCursor(bool shouldBeVisible)
{
    _cursor_locked = shouldBeVisible;
    Window::SetCursorMode(_cursor_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void EventsHandler::PullEvents()
{
    _current++;
    deltaX = 0.0f;
    deltaY = 0.0f;
    glfwPollEvents();
}