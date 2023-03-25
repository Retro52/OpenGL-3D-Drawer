//
// Created by Anton on 02.10.2022.
//

#ifndef GRAPHICS_EVENT_HPP
#define GRAPHICS_EVENT_HPP

#include <queue>
#include <vector>
#include <iostream>
#include "../Core/EngineException.h"

namespace EventTypes
{
    enum EventType
    {
        KeyPressed,
        MouseMoved,
        KeyReleased,
        WindowResized,
        MouseButtonPressed,
        MouseButtonReleased,
    };
}

class Event
{
public:
    virtual ~Event() = default;

    virtual EventTypes::EventType GetType() = 0;

    template<typename T>
    static std::shared_ptr<T> EventCast(const std::shared_ptr<Event>& baseEvent)
    {
        if (typeid(* baseEvent) == typeid(T))
        {
            return std::dynamic_pointer_cast<T>(baseEvent);
        }
        throw EngineException("Event cast failed, type id`s does not match.");
    }
};

class BaseKeyEvent : public Event
{
public:
    explicit BaseKeyEvent(unsigned int keyCode) : keyCode(keyCode) {}

    [[nodiscard]] unsigned int GetKeyCode() const { return keyCode; }

protected:
    unsigned int keyCode;
};

class KeyPressedEvent : public BaseKeyEvent
{
public:
    explicit KeyPressedEvent(unsigned int keyCode) : BaseKeyEvent(keyCode) {}

    EventTypes::EventType GetType() override
    {
        return EventTypes::KeyPressed;
    }
};


class KeyReleasedEvent : public BaseKeyEvent
{
public:
    explicit KeyReleasedEvent(unsigned int keyCode) : BaseKeyEvent(keyCode) {}

    EventTypes::EventType GetType() override
    {
        return EventTypes::KeyReleased;
    }
};

class MouseClickBaseEvent : public Event
{
public:
    explicit MouseClickBaseEvent(unsigned int buttonCode) : buttonCode(buttonCode) {}

    [[nodiscard]] unsigned int GetButtonCode() const { return buttonCode; }

protected:
    unsigned int buttonCode;
};

class MouseButtonClick : public MouseClickBaseEvent
{
public:
    explicit MouseButtonClick(unsigned int buttonCode) : MouseClickBaseEvent(buttonCode) {}

    EventTypes::EventType GetType() override
    {
        return EventTypes::MouseButtonPressed;
    }
};

class MouseButtonRelease : public MouseClickBaseEvent
{
public:
    explicit MouseButtonRelease(unsigned int buttonCode) : MouseClickBaseEvent(buttonCode) {}

    EventTypes::EventType GetType() override
    {
        return EventTypes::MouseButtonReleased;
    }
};

class MouseMoveEvent : public Event
{
public:
    explicit MouseMoveEvent(double deltaX, double deltaY) : deltaX(deltaX), deltaY(deltaY) {}

    EventTypes::EventType GetType() override
    {
        return EventTypes::MouseMoved;
    }

    [[nodiscard]] inline double GetDeltaX() const {return deltaX; }
    [[nodiscard]] inline double GetDeltaY() const {return deltaY; }
private:
    double deltaX, deltaY;
};

class WindowResizeEvent : public Event
{
public:
    WindowResizeEvent(int width, int height) : width(width), height(height) {}

    EventTypes::EventType GetType() override
    {
        return EventTypes::WindowResized;
    }

    [[nodiscard]] int GetWidth() const { return width; }
    [[nodiscard]] int GetHeight() const { return height; }
private:
    int width, height;
};

class EventsStack
{
public:
    static void Emplace(const std::shared_ptr<Event>& event)
    {
        events.push(event);
    }

    static std::queue<std::shared_ptr<Event>>& GetEvents()
    {
        return events;
    }

private:
    static std::queue<std::shared_ptr<Event>> events;
};

inline std::queue<std::shared_ptr<Event>> EventsStack::events;

#endif //GRAPHICS_EVENT_HPP
