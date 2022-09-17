//
// Created by Anton on 08.09.2022.
//

#ifndef GRAPHICS_ACTIONSMAPPING_H
#define GRAPHICS_ACTIONSMAPPING_H

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include "EventsHandler.h"
#include "json.hpp"
#include "../Core/InGameException.h"

class ActionsMapping
{
public:
    inline static bool IsActionJustPressed(const std::string& actionName)
    {
        return std::any_of(actionMap[actionName].begin(), actionMap[actionName].end(), EventsHandler::IsJustPressed);
    }

    inline static bool IsActionPressed(const std::string& actionName)
    {
        return std::any_of(actionMap[actionName].begin(), actionMap[actionName].end(), EventsHandler::IsPressed);
    }

    static void LoadActionMapping(const std::string& path)
    {
        /* Not implemented */
//        std::ifstream is(path);
//        GAME_ASSERT(is.is_open(), "Failed to open controls file " + path);

    }
private:
    static std::unordered_map<std::string, std::vector<int>> actionMap;

    static int FromStringToActionCode(const std::string& actionKeyName)
    {
        if (actionKeyName.length() == 1)
        {
            return std::toupper(actionKeyName.at(0));
        }
        if (to_lower(actionKeyName) == "tab")
        {
            return GLFW_KEY_TAB;
        }
        if (to_lower(actionKeyName) == "space")
        {
            return GLFW_KEY_SPACE;
        }
        if (to_lower(actionKeyName) == "escape")
        {
            return GLFW_KEY_ESCAPE;
        }
        return -1;
    }

    static inline std::string to_lower(const std::string& src)
    {
        std::stringstream res;
        for (char i : src)
        {
            res << std::tolower(i);
        }
        return res.str();
    }
};


#endif //GRAPHICS_ACTIONSMAPPING_H
