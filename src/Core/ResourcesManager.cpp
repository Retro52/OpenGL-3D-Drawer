//
// Created by Anton on 20.12.2021.
//

#include <memory>
#include <iostream>
#include "ResourcesManager.h"
#include "../Entity/Entity.h"

std::unique_ptr<Scene> ResourcesManager::pScene;
std::vector<std::shared_ptr<Layer>> ResourcesManager::layers;
std::map<std::string, std::shared_ptr<Shader>> ResourcesManager::shaders;

std::mutex ResourcesManager::m;

void ResourcesManager::RegisterShader(const std::string &name, const std::string &vFile, const std::string &fFile, const std::string &gFile)
{
    const std::lock_guard<std::mutex> lock(m);
    try
    {
        if (!gFile.empty())
        {
            shaders[name] = std::make_shared<Shader>(vFile.c_str(), fFile.c_str(), gFile.c_str());
        }
        else
        {
            shaders[name] = std::make_shared<Shader>(vFile.c_str(), fFile.c_str());
        }
        LOG(INFO) << "Shader " + name + " successfully registered";
    }
    catch(EngineException& e)
    {
        LOG(WARNING) << "Failed to register shader" << name << ". Reason: " << e.what();
    }
}

std::shared_ptr<Shader>& ResourcesManager::GetShader(const std::string& name)
{
    return shaders.at(name);
}

void ResourcesManager::RegisterPlayerScene(const std::string &path)
{
    const std::lock_guard<std::mutex> lock(m);
    try
    {
        pScene = std::make_unique<Scene>(path);
    }
    catch(std::exception& e)
    {
        std::cerr << e.what();
    }
}
