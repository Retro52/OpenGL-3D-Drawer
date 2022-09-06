//
// Created by Anton on 17.08.2022.
//

#include "Window.h"
#include "Config.h"
#include "ResourcesManager.h"
#include "InGameException.h"
#include "../Loaders/inipp.h"
#include "../Loaders/json.hpp"
#include "../Logging/easylogging++.h"
#include "../UI/UIHandler.h"


void Config::LoadIni(const std::string &configPath)
{
    LOG(INFO) << "Start loading "<< configPath << " file";
    int windowHeight       = 900;
    int windowWidth        = 1200;
    int defaultFontSize    = 16;
    bool windowFullScreen  = false;
    std::string windowName        = "OpenGL Drawer";
    std::string defaultFontPath   = "../res/fonts/arial/arial.ttf";
    std::string defaultScenePath  = "../res/scenes/defaultScene.json";
    std::string shadersConfigPath = "config.json";

    std::ifstream is(configPath);

    GAME_ASSERT(is.is_open(), ".ini config file not found");

    inipp::Ini<char> ini;

    ini.parse(is);
    ini.default_section(ini.sections["DEFAULT"]);
    ini.interpolate();

    inipp::get_value(ini.sections["DEFAULT"], "windowName", windowName);
    inipp::get_value(ini.sections["DEFAULT"], "windowWidth", windowWidth);
    inipp::get_value(ini.sections["DEFAULT"], "windowHeight", windowHeight);
    inipp::get_value(ini.sections["DEFAULT"], "windowFullScreen", windowFullScreen);
    inipp::get_value(ini.sections["DEFAULT"], "defaultFontSize", defaultFontSize);
    inipp::get_value(ini.sections["DEFAULT"], "defaultFontPath", defaultFontPath);
    inipp::get_value(ini.sections["DEFAULT"], "defaultScenePath", defaultScenePath);

    windowWidth = windowWidth > 399 ? windowWidth : 400;
    windowHeight = windowHeight > 399 ? windowHeight : 400;

    inipp::get_value(ini.sections["SHADERS"], "shadersConfigPath", shadersConfigPath);

    is.close();


    LOG(INFO) << configPath << " successfully loaded";
    /* All the exceptions are handled in Global::Init method */
    Window::Initialize(windowWidth, windowHeight, windowName, windowFullScreen);

    Config::LoadJson(shadersConfigPath);
    UIHandler::Initialize(defaultFontPath, defaultFontSize);
    ResourcesManager::RegisterPlayerScene(defaultScenePath);
}

void Config::LoadJson(const std::string &jsonConfigPath)
{
    LOG(INFO) << "Start loading " << jsonConfigPath << " config file";
    std::ifstream is(jsonConfigPath);

    GAME_ASSERT(is.is_open(), "Failed to load .json config file");

    json data = json::parse(is);

    GAME_ASSERT(!data.empty(), "Json file is empty");

    for (const auto& shader : data["Shaders"].items())
    {
        if(!shader.value()[2].is_null())
        {
            ResourcesManager::RegisterShader(shader.key(), shader.value()[0], shader.value()[1], shader.value()[2]);
        }
        else
        {
            ResourcesManager::RegisterShader(shader.key(), shader.value()[0], shader.value()[1]);
        }
    }
}
